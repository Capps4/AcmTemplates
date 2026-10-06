"""Report current correctness coverage and sequential performance measurements."""
from collections import Counter, defaultdict
import datetime
import json
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.Common import ROOT, FLAGS, compiler, fingerprint, manifest, benchmark_path, write_json
from tools.Run import current, load_state, module_jobs, tools_inputs
from tools import REPORTS, SOURCE, RIGHT_PROFILES as PROFILES


def collect():
    modules, state = manifest(), load_state()
    expected = {job['id'] for name in modules for profile in PROFILES
                for job in module_jobs(name, profile)}
    expected.update(f'Integration/{profile}/{label}' for profile in PROFILES
                    for label in ('library', 'options'))
    if tools_inputs():
        expected.add('Tools/right')
    errors, records = [], []
    for ident in sorted(expected):
        record = state.get(ident)
        if not record or record.get('status') != 'passed' or not current(record):
            errors.append('Missing/failed/stale correctness: ' + ident)
        else:
            records.append(record)
    catalog = json.loads((ROOT / 'tests/Correctness/Cases.json').read_text())['addedCases']
    counts = Counter(case['header'] for case in catalog)
    actual = {str(p.relative_to(ROOT)) for p in SOURCE.rglob('code.hpp')}
    actual.update(str(p.relative_to(ROOT)) for p in (SOURCE / 'Geometry/Geo2').glob('*.hpp')
                  if p.name != 'Include.hpp')
    if set(counts) != actual or any(n < 10 for n in counts.values()):
        errors.append('Each algorithm header must have at least 10 catalogued cases')
    required = {case['id'] for case in catalog}
    if len(required) != len(catalog):
        errors.append('Duplicate case IDs')
    for case in catalog:
        if not (ROOT / case['source']).is_file():
            errors.append('Missing case source: ' + case['source'])
    named = defaultdict(set)
    for record in records:
        if record['signature'].get('kind') == 'tools':
            continue
        for run in record.get('executions', []):
            for line in run['output'].splitlines():
                if line.startswith('CASE_PASS '):
                    named[record['signature']['profile']].add(line[10:])
    for profile in PROFILES:
        if not required.issubset(named[profile]):
            errors.append('Incomplete named case execution: ' + profile)

    latest = REPORTS / 'LatestBenchmark.json'
    bench, bench_path, environment = [], None, None
    if latest.exists():
        bench_path = ROOT / json.loads(latest.read_text())['report']
        data = json.loads(bench_path.read_text())
        bench, environment = data['records'], data['hardware']
    expected_bench = {name + '/' + p.stem for name in modules
                      for p in benchmark_path(name).glob('Benchmark*.cpp')}
    covered = {record['module'] for record in bench}
    if covered != set(modules) or {r['id'] for r in bench} != expected_bench:
        errors.append('Performance coverage does not match the module/benchmark inventory')
    for record in bench:
        try:
            sig, samples = record['signature'], record['samples']
            workload = {(s['n'], s['shape']) for s in samples}
            valid = (record['status'] == 'passed' and len(samples) == 4 and len(workload) == 4
                     and {s['shape'] for s in samples} == {0, 1}
                     and len({s['n'] for s in samples}) == 2
                     and all(len(s['samplesMs']) == 7 for s in samples)
                     and sig['compiler'] == compiler('optimized') and sig['flags'] == FLAGS['optimized']
                     and fingerprint([ROOT / p for p in sig['sha256']]) == sig['sha256'])
        except (KeyError, OSError, RuntimeError, ValueError):
            valid = False
        record['current'] = valid
        if not valid:
            errors.append('Missing/failed/stale performance: ' + record['id'])
    return {'passed': not errors, 'errors': errors, 'modules': len(modules),
            'correctnessJobs': len(expected), 'freshCorrectnessJobs': len(records),
            'namedCases': len(catalog), 'headers': len(actual), 'caseCounts': dict(counts),
            'benchmarks': bench, 'hardware': environment,
            'benchmarkReport': str(bench_path.relative_to(ROOT)) if bench_path else None}


def report(data):
    status = '通过' if data['passed'] else '未完成'
    lines = ['# 验证报告', '', f'生成时间：{datetime.datetime.now().astimezone().isoformat(timespec="seconds")}；状态：**{status}**。', '',
             f"{data['modules']} 个模块、{data['headers']} 个算法头；具名用例 {data['namedCases']} 个。"
             f"正确性任务 {data['correctnessJobs']} 个，现版有效通过记录 {data['freshCorrectnessJobs']} 个。", '',
             '正确性使用 strict、optimized、sanitized 三种配置，包含各模块的暴力对照、边界、随机、规模、示例及组合测试。'
             'strict 参数见 tests/README.md；只接受源码、依赖、编译器和参数仍匹配的记录。', '',
             '性能输入生成不计时，预热一次、采样七次，中位数计时；进程顺序执行。'
             'RSS 为整进程峰值，包含输入和运行时。重复校验值稳定只能证明重复执行一致，正确性仍依赖独立 oracle。'
             '两档规模、两种形状不等于穷尽所有 API 或支持域，本机结果不证明 OJ 前 5%。', '',
             '递归图算法的深链测试使用 256 MiB 线程栈；通过记录不代表默认 OJ 栈足够。', '',
             '性能环境：`' + json.dumps(data['hardware'], ensure_ascii=False) + '`。', '',
             '| 用例 | 较大规模 n | 两种形状中位数 ms | 峰值 RSS MiB |', '| --- | ---: | ---: | ---: |']
    for record in data['benchmarks']:
        samples = record.get('samples', [])
        if not record.get('current') or not samples:
            lines.append(f"| {record['id']} | — | {record['status'] if record.get('current') else '待验证/过期'} | — |")
            continue
        n = max(s['n'] for s in samples)
        large = sorted((s for s in samples if s['n'] == n), key=lambda s: s['shape'])
        timings = ' / '.join(f"{s['medianMs']:.3f}" for s in large)
        rss = max((s.get('peakRssBytes') or 0) for s in large) / (1 << 20)
        lines.append(f"| {record['id']} | {n} | {timings} | {rss:.1f} |")
    if data['benchmarkReport']:
        lines.extend(['', '原始性能记录：`' + data['benchmarkReport'] + '`。'])
    if data['errors']:
        lines.extend(['', '待完成：', '', *['- ' + error for error in data['errors']]])
    return '\n'.join(lines) + '\n'


def save():
    data = collect()
    write_json(REPORTS / 'Verification.json', data)
    dest = REPORTS / 'Verification.md'
    dest.write_text(report(data), encoding='utf-8')
    return data, dest
