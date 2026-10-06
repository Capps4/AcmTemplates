"""One readable report containing compact, self-contained publication evidence."""
import base64
from collections import Counter, defaultdict
import datetime
import json
from pathlib import Path
import re
import shutil
import zlib

import tools as config
from tools.Common import ROOT, catalog, manifest, benchmark_path, hardware, include_digests, save_digest_cache
from tools.Run import current, module_jobs, tools_inputs
from tools.Benchmark import current as benchmark_current

PROFILES = ('strict', 'optimized', 'sanitized')
MARKER = 'ctl-evidence-v4:'


def read_evidence():
    path = config.REPORTS / 'Verification.md'
    try:
        match = re.search(r'<!-- ' + MARKER + r' ([A-Za-z0-9+/=]+) -->', path.read_text())
        if not match:
            return {}
        result = json.loads(zlib.decompress(base64.b64decode(match[1])))
        return result if isinstance(result, dict) and result.get('formatVersion') == 4 else {}
    except (OSError, ValueError, zlib.error):
        return {}


def collect(evidence=None):
    evidence = read_evidence() if evidence is None else evidence
    modules, items = manifest(), catalog()
    state = {r['id']: r for r in evidence.get('right', [])}
    benchmarks = {r['id']: r for r in evidence.get('perf', [])}
    actual_digests = include_digests([*state.values(), *benchmarks.values()])
    expected_jobs = {job['id']: job for name in modules for profile in PROFILES for job in module_jobs(name, profile)}
    expected = set(expected_jobs)
    if tools_inputs():
        expected.add('Tools/right')
    valid = {ident: state[ident] for ident in expected
             if ident in state and state[ident].get('status') == 'passed' and current(state[ident], job=expected_jobs.get(ident), actual_digests=actual_digests)}
    errors = []
    catalog_path = ROOT / 'tests/Correctness/Cases.json'
    try:
        cases = json.loads(catalog_path.read_text())['addedCases']
    except (OSError, ValueError, KeyError):
        cases = []
        errors.append('正确性覆盖清单缺失或无效')
    counts = Counter(c['header'] for c in cases)
    if set(counts) != {str(item['path'].relative_to(ROOT)) for item in items.values()}:
        errors.append('覆盖清单与算法头不一致')
    if len({c['id'] for c in cases}) != len(cases):
        errors.append('重复 case ID')
    for case in cases:
        if not (ROOT / case['source']).is_file() or not case.get('scenario') or not case.get('oracle'):
            errors.append('缺少 case 来源、场景或 oracle：' + case['id'])
    named = defaultdict(set)
    for record in valid.values():
        for ident in record.get('caseIds', []):
            named[record.get('profile')].add(ident)
    expected_bench = {name + '/' + p.stem for name in modules for p in benchmark_path(name).glob('Benchmark*.cpp')}
    fresh_bench = {ident: r for ident, r in benchmarks.items() if ident in expected_bench and benchmark_current(r)}
    rows = []
    for key, item in sorted(items.items()):
        name = item['module']
        required = {c['id'] for c in cases if c['header'] == str(item['path'].relative_to(ROOT))}
        right_jobs = {ident for ident in expected if ident.startswith(name + '/')}
        right_ok = bool(required) and right_jobs.issubset(valid) and all(required.issubset(named[p]) for p in PROFILES)
        perf_jobs = {ident for ident in expected_bench if ident.startswith(name + '/')}
        perf_ok = bool(perf_jobs) and perf_jobs.issubset(fresh_bench)
        # Layered modules use a benchmark with the layer name in its stem.
        layer_bench = [r for r in fresh_bench.values() if r['module'] == name and
                       (key == name or item['name'] in Path(r['source']).stem)]
        if not layer_bench:
            layer_bench = [r for r in fresh_bench.values() if r['module'] == name]
        perf_ms = sum(sum(s.get('samplesMs', [])) + s.get('warmupMs', 0)
                      for r in layer_bench for s in r['samples'])
        profile_ms = {p: sum(ms for r in valid.values() if r['module'] == name and r.get('profile') == p
                             for ident, ms in r.get('caseMs', {}).items() if ident in required) for p in PROFILES}
        rows.append({'name': key, 'module': name, 'right': right_ok, 'perf': perf_ok,
                     'correctnessMs': profile_ms, 'performanceMs': perf_ms,
                     'performanceSkipped': perf_ok and all(fresh_bench[ident]['status'] == 'skipped' for ident in perf_jobs)})
        if not right_ok:
            errors.append('正确性缺失、失败或过期：' + key)
        if not perf_ok:
            errors.append('性能缺失、失败或过期：' + key)
    tools_ok = not tools_inputs() or 'Tools/right' in valid
    if not tools_ok:
        errors.append('工具回归缺失、失败或过期：Tools/right')
    failures = [r for r in [*state.values(), *benchmarks.values()] if r.get('status') != 'passed' and not (r.get('status') == 'skipped' and benchmark_current(r))]
    over_budget = [r['name'] + ': right=' + format(r['correctnessMs']['optimized'], '.2f') + 'ms, perf=' + format(r['performanceMs'], '.2f') + 'ms'
                   for r in rows if (r['right'] and r['correctnessMs']['optimized'] > config.CORRECTNESS_TARGET_MS[1]) or
                   (r['perf'] and r['performanceMs'] > config.PERFORMANCE_TARGET_MS[1])]
    return {'overBudget': over_budget, 'passed': not errors, 'errors': errors, 'rows': rows, 'toolsPassed': tools_ok,
            'failures': failures, 'modules': len(modules), 'headers': len(items),
            'namedCases': len(cases), 'correctnessJobs': len(expected),
            'freshCorrectnessJobs': len(valid)}


def failure_text(record):
    label = record['id'] + ' · ' + record.get('phase', 'unknown')
    if record.get('failedCase'):
        label += ' · case=' + record['failedCase']
    if record.get('signature', {}).get('seed') is not None:
        label += ' · seed=' + str(record['signature']['seed'])
    if record.get('workload'):
        label += ' · ' + json.dumps(record['workload'], ensure_ascii=False)
    if record.get('module') not in ('Tools', 'Selection'):
        sig = record.get('signature', {})
        mode = '--right' if record.get('profile') else '--perf'
        replay = 'ctl test ' + mode + ' --module ' + record['module']
        if record.get('profile'): replay += ' --profile ' + record['profile']
        if record.get('failedCase'): replay += ' --case ' + record['failedCase']
        if sig.get('seed') is not None: replay += ' --seed ' + str(sig['seed'])
        label += '\n复现：' + replay
    return label + '\n' + record.get('error', '未完成')


def report(data):
    status = '通过' if data['passed'] else '未完成'
    lines = ['# 测试报告', '', f'{data["headers"]} 个模板 · {status}', '',
             '| 模板 | 结果 |', '| --- | --- |']
    for row in data['rows']:
        result = '✓' if row['right'] and row['perf'] else '待验证/失败'
        if row['performanceSkipped']:
            result += '（无需性能测试）'
        lines.append(f'| {row["name"]} | {result} |')
    lines.extend(['', 'CTL 工具回归：' + ('✓' if data['toolsPassed'] else '待验证/失败')])
    if data['failures']:
        for record in data['failures']:
            lines.extend(['', '```text', failure_text(record).replace('```', "'''"), '```'])
    coverage_errors = [error for error in data['errors'] if not error.startswith(('正确性缺失', '性能缺失', '工具回归缺失'))]
    if coverage_errors:
        lines.extend(['', *['- ' + error for error in coverage_errors]])
    if data['overBudget']:
        lines.extend(['', '待校准（超过目标上限）：' + '；'.join(data['overBudget'])])
    lines.extend(['', '耗时以优化版 case 工作量和性能全部预热/采样工作量计；编译、启动、strict/Sanitizer 开销单列在内嵌证据中。'])
    return '\n'.join(lines) + '\n'


def cleanup_legacy():
    # These paths are owned by testing. Generated documents, publishing backups
    # and rule reports belong to other workflows and are never removed here.
    for name in ('build', 'benchmarks'):
        path = config.CACHE / name
        if path.is_dir() and not path.is_symlink():
            shutil.rmtree(path)
    for name in ('State.json', 'Latest.json', 'LatestBenchmark.json', 'Verification.json'):
        (config.REPORTS / name).unlink(missing_ok=True)
    if config.REPORTS.exists():
        for path in config.REPORTS.iterdir():
            legacy = (re.fullmatch(r'\d{8}T\d{6}Z-(?:(?:benchmark|special)-)?[0-9a-f]{8}', path.name) or
                      re.fullmatch(r'benchmark-merged-\d{8}T\d{6}Z', path.name))
            if legacy and path.is_dir() and not path.is_symlink():
                shutil.rmtree(path)


def save(right=None, perf=None):
    evidence = read_evidence()
    for key, records in (('right', right), ('perf', perf)):
        if records is None:
            continue
        replaced = {r['module'] for r in records}
        old = [r for r in evidence.get(key, []) if r['module'] not in replaced and r['module'] != 'Selection']
        evidence[key] = sorted([*old, *records], key=lambda r: r['id'])
    evidence['formatVersion'] = 4
    if right is not None or perf is not None:
        evidence['hardware'] = hardware()
    evidence['generatedAt'] = datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=8))).isoformat(timespec='seconds')
    data = collect(evidence)
    save_digest_cache()
    encoded = base64.b64encode(zlib.compress(json.dumps(evidence, ensure_ascii=False, separators=(',', ':')).encode())).decode()
    config.REPORTS.mkdir(parents=True, exist_ok=True)
    dest = config.REPORTS / 'Verification.md'
    tmp = dest.with_suffix('.md.tmp')
    tmp.write_text(report(data) + '\n<!-- ' + MARKER + ' ' + encoded + ' -->\n', encoding='utf-8')
    tmp.replace(dest)
    if right is not None or perf is not None:
        cleanup_legacy()
    return data, dest
