"""Reproducible GNU++17 checks and benchmarks. See README.md for the workflow."""
import argparse
import concurrent.futures
import csv
import datetime
import fcntl
import io
import json
import math
import os
import platform
import shutil
import statistics
import sys
import uuid
from pathlib import Path
from Common import ROOT, FLAGS, compiler, hardware, digest, execute, fingerprint, headers, manifest, write_json

REPORTS = ROOT / 'Reports'
BUILD = ROOT / 'Build'
STATE = REPORTS / 'State.json'


def load_state():
    return json.loads(STATE.read_text()) if STATE.exists() else {}


def signature(job, comp, seed):
    return {'sha256': fingerprint(job['inputs'] + [ROOT / 'Run.py', ROOT / 'Common.py', ROOT / 'Manifest.json']),
            'compiler': comp, 'flags': FLAGS[job['profile']], 'cases': [list(c) for c in job['cases']],
            'sources': [str(p.relative_to(ROOT)) for p in job['sources']],
            'seed': seed, 'profile': job['profile'],
            'hardware': hardware() if job['profile'] == 'benchmark' else None,
            'environment': {key: os.environ.get(key) for key in
                            ('CPATH', 'CPLUS_INCLUDE_PATH', 'LIBRARY_PATH', 'SDKROOT', 'MACOSX_DEPLOYMENT_TARGET')}}


def current(record):
    try:
        sig = record['signature']
        paths = [ROOT / name for name in sig['sha256']]
        if record['module'] != 'Integration' and sig['profile'] != 'benchmark':
            job = next((j for j in module_jobs(record['module'], sig['profile']) if j['id'] == record['id']), None)
            if not job or [str(p.relative_to(ROOT)) for p in job['sources']] != sig['sources']:
                return False
        return (fingerprint(paths) == sig['sha256'] and
                compiler(sig['profile']) == sig['compiler'] and
                FLAGS[sig['profile']] == sig['flags'] and
                (sig['profile'] != 'benchmark' or sig.get('hardware') == hardware()) and
                all(os.environ.get(k) == v for k, v in sig.get('environment', {}).items()) and
                all(digest(ROOT / p) == val for p, val in record.get('actualIncludes', {}).items()))
    except (KeyError, OSError, RuntimeError, ValueError):
        return False


def module_jobs(name, profile):
    folder = ROOT / name
    sources = [folder / 'Test.cpp', *sorted(folder.glob('TestExtra*.cpp'))]
    groups = [('main', sources)] + [(p.stem, [p]) for p in sorted(folder.glob('TestCase*.cpp'))]
    return [{'id': f'{name}/{profile}/{label}', 'module': name, 'profile': profile,
             'sources': src, 'inputs': src + headers(name), 'cases': [(None, None)]}
            for label, src in groups]


def integration_jobs(profile):
    from Integration import inputs, assemble
    from Options import MODULES, source
    modules = inputs()
    folder = BUILD / 'Sources'
    folder.mkdir(parents=True, exist_ok=True)
    all_headers = [ROOT / name / 'Final.hpp' for name in modules]
    common = [ROOT / 'Integration.py', ROOT / 'Manifest.json', *all_headers]
    jobs = []
    for name, text, cases, paths in [
        ('library', assemble(modules), [(None, None)], [ROOT / 'Integration/Test.hpp']),
        ('options', source(), [(None, None)], [ROOT / 'Options.py',
                                              *[ROOT / name / 'Final.hpp' for name in MODULES]])
    ]:
        path = folder / (name + '.cpp')
        if not path.exists() or path.read_text() != text:
            path.write_text(text)
        jobs.append({'id': f'Integration/{profile}/{name}', 'module': 'Integration',
                     'profile': profile, 'sources': [path], 'inputs': common + paths,
                     'cases': cases})
    return jobs


def compile_job(job, comp, timeout):
    folder = BUILD / job['id']
    folder.mkdir(parents=True, exist_ok=True)
    commands, objects, actual = [], [], set()
    diagnostics = []
    for i, source in enumerate(job['sources']):
        obj, dep = folder / f'{i}.o', folder / f'{i}.d'
        cmd = [comp['path'], *FLAGS[job['profile']], '-MMD', '-MF', str(dep),
               '-c', str(source), '-o', str(obj)]
        result = execute(cmd, timeout=timeout)
        diagnostics.append(result.stdout + result.stderr)
        commands.append(cmd)
        objects.append(str(obj))
        # GCC/Clang escape spaces in make dependency paths. Parse the compiler's
        # actual includes as well as the conservative quoted-include fingerprint.
        import shlex
        text = dep.read_text().replace('\\\n', ' ')
        for name in shlex.split(text.split(':', 1)[1]):
            path = Path(name).resolve()
            if path.is_relative_to(ROOT) and not path.is_relative_to(BUILD) and path.is_file():
                actual.add(path)
    target = folder / 'program'
    cmd = [comp['path'], *FLAGS[job['profile']], *objects, '-o', str(target)]
    result = execute(cmd, timeout=timeout)
    diagnostics.append(result.stdout + result.stderr)
    commands.append(cmd)
    return target, commands, sorted(actual), '\n'.join(diagnostics)


def verify_job(job, comp, seed, previous, run_dir, timeout, run_tests, force):
    record = {'id': job['id'], 'module': job['module'], 'cached': False,
              'status': 'failed', 'commands': [], 'executions': []}
    log = run_dir / (job['id'].replace('/', '-') + '.log')
    record['log'] = str(log.relative_to(ROOT))
    try:
        sig = signature(job, comp, seed)
        record['signature'] = sig
        old = previous.get(job['id'], {})
        target = BUILD / job['id'] / 'program'
        reuse = (old.get('status') in ('passed', 'compiled') and old.get('signature') == sig
                 and target.exists() and current(old))
        if reuse and not force and (not run_tests or old.get('status') == 'passed'):
            print(job['id'] + ': cached', flush=True)
            return dict(old, cached=True)
        target, commands, actual, diagnostics = compile_job(job, comp, timeout)
        record['commands'] = commands
        record['actualIncludes'] = {str(p.relative_to(ROOT)): digest(p) for p in actual}
        log.write_text(diagnostics)
        if run_tests:
            for text, expected in job['cases']:
                result = execute([str(target)], input_text=text, timeout=timeout,
                                 env={'TEST_SEED': str(seed)})
                with log.open('a') as out:
                    out.write(result.stdout + result.stderr)
                if expected is not None and result.stdout != expected:
                    raise RuntimeError(f'Wrong output for input {text!r}: {result.stdout!r}, expected {expected!r}')
                record['executions'].append({'input': text, 'output': result.stdout, 'stderr': result.stderr})
        if signature(job, comp, seed) != sig:
            raise RuntimeError('Inputs changed during verification; rerun')
        record['status'] = 'passed' if run_tests else 'compiled'
    except Exception as exc:
        record['error'] = str(exc)
        with log.open('a') as out:
            out.write('\n' + str(exc))
    print(job['id'] + ': ' + record['status'], flush=True)
    return record


def check_samples(output, samples):
    rows = list(csv.reader(io.StringIO(output)))
    if not rows or len(rows) != len(samples):
        raise RuntimeError('Missing benchmark samples')
    for row, sample in zip(rows, samples):
        if len(row) != 5 or row[0] != sample['scenario'] or int(row[4]) % (1 << 64) != sample['checksum']:
            raise RuntimeError('Invalid benchmark checksum or scenario')
        med = []
        for field, col in [('original', 1), ('final', 2)]:
            vals = sample[field]
            if len(vals) != 7 or not all(math.isfinite(x) and x >= 0 for x in vals):
                raise RuntimeError('Invalid benchmark samples')
            m = statistics.median(vals)
            if not math.isclose(m, float(row[col]), rel_tol=6e-6, abs_tol=1e-9):
                raise RuntimeError('Benchmark median differs from raw samples')
            med.append(m)
        if all(med):
            if not math.isclose(med[0] / med[1], float(row[3]), rel_tol=6e-6):
                raise RuntimeError('Invalid speedup')
        elif row[3] != 'NA':
            raise RuntimeError('Invalid zero-duration ratio')


def spread(values):
    med = statistics.median(values)
    return statistics.median(abs(x - med) for x in values) / med if med else 1


def benchmark_jobs(names, comp, args, run_dir):
    # Finish every compilation before starting any timed process.
    prepared = []
    records = []
    snapshot = None
    if args.baseline != 'original':
        snapshot = load_baseline(args.baseline)
    for name in names:
        job = {'id': f'{name}/benchmark/main', 'module': name, 'profile': 'benchmark',
               'sources': [ROOT / name / 'Benchmark.cpp'], 'inputs': [], 'cases': []}
        job['inputs'] = job['sources'] + headers(name)
        sig = signature(job, comp, args.seed)
        sig['minMs'] = args.min_ms
        record = {'id': job['id'], 'module': name, 'status': 'failed', 'signature': sig}
        records.append(record)
        try:
            target, commands, actual, diagnostics = compile_job(job, comp, args.timeout)
            record.update(commands=commands, actualIncludes={str(p.relative_to(ROOT)): digest(p) for p in actual})
            (run_dir / (name + '-compile.log')).write_text(diagnostics)
            old_target = None
            if snapshot:
                baseline_record(snapshot, record)
                old_source = ROOT / 'Baselines' / args.baseline / 'Source' / name / 'Benchmark.cpp'
                old_target = BUILD / job['id'] / 'baseline'
                cmd = [comp['path'], *FLAGS['benchmark'], str(old_source), '-o', str(old_target)]
                result = execute(cmd, timeout=args.timeout)
                record['baselineCompile'] = cmd
            prepared.append((job, target, record, old_target))
        except Exception as exc:
            record['error'] = str(exc)
    for job, target, record, old_target in prepared:
        try:
            result = execute([str(target)], timeout=args.timeout,
                             env={'BENCH_MIN_MS': str(args.min_ms), 'BENCH_ROUNDS': '7',
                                  'BENCH_SIDE': '', 'TEST_SEED': str(args.seed)})
            (run_dir / (job['module'] + '-benchmark.log')).write_text(result.stdout + result.stderr)
            samples = [json.loads(line.split(':', 1)[1]) for line in result.stderr.splitlines()
                       if line.startswith('BENCHMARK_SAMPLES:')]
            check_samples(result.stdout, samples)
            sig = signature(job, comp, args.seed)
            sig['minMs'] = args.min_ms
            if sig != record['signature']:
                raise RuntimeError('Inputs changed during benchmark')
            record.update(status='passed', benchmark=result.stdout, samples=samples)
            if args.baseline != 'original':
                record['baselineComparison'] = measure_snapshot(target, old_target, args)
            (run_dir / (job['module'] + '.csv')).write_text(
                'scenario,originalMs,finalMs,speedup,checksum\n' + result.stdout)
        except Exception as exc:
            record['error'] = str(exc)
        print(job['id'] + ': ' + record['status'], flush=True)
    return records


def load_baseline(name):
    path = ROOT / 'Baselines' / name
    report = json.loads((path / 'Results.json').read_text())
    for rel, val in report['sha256'].items():
        if digest(path / 'Source' / rel) != val:
            raise RuntimeError('Baseline snapshot was modified: ' + rel)
    return report


def baseline_record(snapshot, record):
    old = next((r for r in snapshot['records'] if r['id'] == record['id']), None)
    if not old or old['status'] != 'passed':
        raise RuntimeError('Baseline has no passed benchmark for ' + record['module'])
    a, b = old['signature'], record['signature']
    if snapshot.get('hardware') != hardware() or any(a[k] != b[k] for k in ('compiler', 'flags', 'seed', 'minMs')):
        raise RuntimeError('Baseline machine/compiler/flags/seed/timing configuration differs')
    for rel in ['BenchmarkSupport.hpp', record['module'] + '/Benchmark.cpp']:
        if a['sha256'].get(rel) != b['sha256'].get(rel):
            raise RuntimeError('Benchmark scenario or timing helper changed; baseline is not comparable')
    return old


def measure_snapshot(target, old_target, args):
    rounds = []
    env = {'BENCH_MIN_MS': str(args.min_ms), 'BENCH_ROUNDS': '1',
           'BENCH_SIDE': 'final', 'TEST_SEED': str(args.seed)}
    for i in range(7):
        row = {}
        order = [('baseline', old_target), ('final', target)]
        if i & 1:
            order.reverse()
        for label, executable in order:
            result = execute([str(executable)], timeout=args.timeout, env=env)
            samples = [json.loads(line.split(':', 1)[1]) for line in result.stderr.splitlines()
                       if line.startswith('BENCHMARK_SAMPLES:')]
            if not samples or any(len(s['final']) != 1 or not math.isfinite(s['final'][0]) or s['final'][0] < 0 for s in samples):
                raise RuntimeError('Invalid one-round baseline samples')
            row[label] = samples
        rounds.append(row)
    names = [s['scenario'] for s in rounds[0]['final']]
    results = []
    for idx, name in enumerate(names):
        prev, now, checksum = [], [], rounds[0]['final'][idx]['checksum']
        for row in rounds:
            for label in ('baseline', 'final'):
                if [s['scenario'] for s in row[label]] != names or row[label][idx]['checksum'] != checksum:
                    raise RuntimeError('Baseline scenario/checksum differs')
            prev.append(row['baseline'][idx]['final'][0])
            now.append(row['final'][idx]['final'][0])
        results.append({'scenario': name, 'original': prev, 'final': now, 'checksum': checksum})
    return {'name': args.baseline, 'method': 'seven process rounds alternating AB/BA; final side only',
            'rounds': rounds, 'samples': results}


def summary_text(report):
    lines = ['# 验证报告', '', f"运行：{report['runId']}；平台：{report['platform']}。", '',
             '暂缓：' + '、'.join(report['deferred']) + '。', '',
             '| 项目 | 状态 | 缓存 |', '| --- | --- | --- |']
    for record in report['records']:
        lines.append(f"| {record['id']} | {record['status']} | {'是' if record.get('cached') else '否'} |")
        if record.get('error'):
            lines.extend(['', '```text', record['error'], '```', ''])
        if record.get('samples'):
            lines.extend(['', '| 场景 | Original ms | Final ms | 倍率 | 波动 MAD/中位数 | 判断 |',
                          '| --- | ---: | ---: | ---: | ---: | --- |'])
            for s in record['samples']:
                a, b = statistics.median(s['original']), statistics.median(s['final'])
                ratio = a / b if b else 0
                noise = max(spread(s['original']), spread(s['final']))
                tiny = min(a, b) * s.get('batch', 1) < 1
                verdict = '尚无明确收益' if tiny or noise > .05 or abs(ratio - 1) < max(.03, 3 * noise) else ('较快' if ratio > 1 else '较慢')
                lines.append(f"| {s['scenario']} | {a:.6g} | {b:.6g} | {ratio:.4g} | {noise:.1%} | {verdict} |")
            lines.extend(['', '毫秒按一次 workload 折算；AB/BA 交替，七轮中位数。波动判断仅作筛选，不是统计显著性检验。', ''])
        if record.get('baselineComparison'):
            comparison = record['baselineComparison']
            lines.extend(['', 'Final 快照基线：' + comparison['name'] + '（重新编译，七轮程序顺序 AB/BA 交替）。', '',
                          '| 场景 | 快照 Final ms | 当前 Final ms | 倍率 | 波动 |', '| --- | ---: | ---: | ---: | ---: |'])
            for row in comparison['samples']:
                a, b = statistics.median(row['original']), statistics.median(row['final'])
                noise = max(spread(row['original']), spread(row['final']))
                ratio = a / b if b else 0
                lines.append(f"| {row['scenario']} | {a:.6g} | {b:.6g} | {ratio:.4g} | {noise:.1%} |")
    return '\n'.join(lines) + '\n'


def save_report(records, args, run_dir, modules):
    report = {'formatVersion': 3, 'runId': run_dir.name, 'platform': platform.platform(),
              'hardware': hardware(), 'command': args.command, 'seed': args.seed,
              'deferred': sorted(name for name, item in modules.items() if item.get('deferred')),
              'records': records}
    write_json(run_dir / 'Results.json', report)
    (run_dir / 'Summary.md').write_text(summary_text(report))
    write_json(REPORTS / 'Latest.json', {'report': str((run_dir / 'Results.json').relative_to(ROOT))})
    state = load_state()
    for record in records:
        state[record['id']] = record
    write_json(STATE, state)
    print('Report: ' + str(run_dir / 'Summary.md'))
    return report


def select(args, modules, previous):
    active = [name for name, item in modules.items() if not item.get('deferred')]
    if args.modules:
        missing = set(args.modules) - set(modules)
        if missing:
            raise RuntimeError('Unknown modules: ' + ', '.join(sorted(missing)))
        deferred = [n for n in args.modules if modules[n].get('deferred')]
        if deferred:
            raise RuntimeError('Deferred modules: ' + ', '.join(deferred))
        return list(dict.fromkeys(args.modules))
    if args.all or args.affected:
        return sorted(active)
    raise RuntimeError('Specify modules, --all or --affected')


def status():
    lines = []
    for key, record in sorted(load_state().items()):
        state = record['status'] if current(record) else 'stale'
        lines.append(f'{key}: {state}')
    print('\n'.join(lines) if lines else 'No verification records yet')
    return lines


def baseline_save(name):
    if not name or not all(c.isalnum() or c in '-_' for c in name):
        raise RuntimeError('Baseline name must contain only letters, digits, - or _')
    dest = ROOT / 'Baselines' / name
    if dest.exists():
        raise RuntimeError('Baseline already exists; snapshots are immutable')
    state = load_state()
    latest = json.loads((ROOT / json.loads((REPORTS / 'Latest.json').read_text())['report']).read_text())
    scope = {r['module'] for r in latest['records'] if '/benchmark/' in r['id'] and r['status'] == 'passed'}
    records = [r for r in state.values() if r['module'] in scope and r['status'] == 'passed' and current(r)]
    if not any('/benchmark/' in r['id'] for r in records):
        raise RuntimeError('Run benchmarks before saving a baseline')
    for record in records:
        if '/benchmark/' in record['id']:
            for profile in ('strict', 'optimized', 'sanitized'):
                for job in module_jobs(record['module'], profile):
                    proof = state.get(job['id'])
                    if not proof or proof['status'] != 'passed' or not current(proof):
                        raise RuntimeError('Baseline requires current passed checks: ' + job['id'])
    files = {}
    for record in records:
        for path, val in {**record['signature']['sha256'], **record.get('actualIncludes', {})}.items():
            if path in files and files[path] != val:
                raise RuntimeError('Inconsistent snapshot evidence: ' + path)
            files[path] = val
    try:
        for path, val in sorted(files.items()):
            src, dst = ROOT / path, dest / 'Source' / path
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src, dst)
            if digest(dst) != val or digest(src) != val:
                raise RuntimeError('Input changed during snapshot: ' + path)
        write_json(dest / 'Results.json', {'platform': platform.platform(), 'hardware': hardware(),
                                         'records': records, 'sha256': files})
    except Exception:
        shutil.rmtree(dest, ignore_errors=True)
        raise
    print('Saved immutable baseline: ' + str(dest))


def package_headers(modules):
    state = load_state()
    required = []
    for name, item in modules.items():
        if not item.get('deferred'):
            for profile in ('strict', 'optimized', 'sanitized'):
                required.extend(j['id'] for j in module_jobs(name, profile))
    for profile in ('strict', 'optimized', 'sanitized'):
        required.extend(j['id'] for j in integration_jobs(profile))
    for key in required:
        record = state.get(key)
        if not record or record['status'] != 'passed' or not current(record):
            raise RuntimeError('Package requires current passed verification: ' + key)
    dest = ROOT / 'ReadyHeaders'
    if dest.exists():
        shutil.rmtree(dest)
    records = []
    for name, item in modules.items():
        if item.get('deferred'):
            continue
        source = ROOT / name / 'Final.hpp'
        target = dest / name / 'Final.hpp'
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
        expected = state[f'{name}/strict/main']['signature']['sha256'][name + '/Final.hpp']
        if digest(target) != expected or digest(source) != expected:
            raise RuntimeError('Header changed during packaging: ' + name)
        records.append({'module': name, 'file': str(target.relative_to(dest)), 'sha256': expected})
    write_json(dest / 'Package.json', records)
    print(f'{len(records)} verified headers: {dest}')


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=['compile', 'test', 'sanitize', 'check', 'bench',
                                         'integration', 'status', 'report', 'baseline', 'package', 'clean'])
    parser.add_argument('modules', nargs='*')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--all', action='store_true')
    group.add_argument('--affected', action='store_true')
    parser.add_argument('--force', action='store_true')
    parser.add_argument('--jobs', type=int, default=3)
    parser.add_argument('--seed', type=int, default=20261001)
    parser.add_argument('--timeout', type=float, default=180)
    parser.add_argument('--min-ms', type=float, default=10)
    parser.add_argument('--baseline', default='original')
    args = parser.parse_args(argv)
    args.command = ['python3', str(ROOT / 'Run.py'), *(sys.argv[1:] if argv is None else argv)]
    if args.jobs < 1 or args.timeout <= 0 or args.min_ms <= 0 or args.seed < 0:
        parser.error('jobs, timeout, min-ms must be positive; seed must be nonnegative')
    REPORTS.mkdir(exist_ok=True)
    # All entrypoints use the same process lock. Compiles/tests can run in the
    # internal pool; a benchmark never overlaps another controlled invocation.
    with (REPORTS / '.run.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        modules, previous = manifest(), load_state()
        if args.action in ('status', 'report'):
            status()
            if args.action == 'report' and (REPORTS / 'Latest.json').exists():
                print(ROOT / json.loads((REPORTS / 'Latest.json').read_text())['report'])
            return
        if args.action == 'clean':
            shutil.rmtree(BUILD, ignore_errors=True)
            print('Build removed; reports and baselines retained')
            return
        if args.action == 'baseline':
            if len(args.modules) != 1:
                parser.error('baseline requires one snapshot name')
            baseline_save(args.modules[0])
            return
        if args.action == 'package':
            package_headers(modules)
            return
        run_id = datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + uuid.uuid4().hex[:8]
        run_dir = REPORTS / run_id
        run_dir.mkdir()
        if args.action == 'bench':
            names = select(args, modules, previous)
            records = benchmark_jobs(names, compiler('benchmark'), args, run_dir)
        else:
            profiles = {'compile': ['strict'], 'test': ['optimized'], 'sanitize': ['sanitized'],
                        'check': ['strict', 'optimized', 'sanitized'],
                        'integration': ['strict', 'optimized', 'sanitized']}[args.action]
            jobs = []
            if args.action != 'integration':
                names = select(args, modules, previous)
                jobs = [job for name in names for profile in profiles for job in module_jobs(name, profile)]
            if args.action == 'integration' or args.all or args.affected:
                jobs.extend(job for profile in profiles for job in integration_jobs(profile))
            compilers = {p: compiler(p) for p in profiles}
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
                futures = [pool.submit(verify_job, job, compilers[job['profile']], args.seed,
                                       previous, run_dir, args.timeout, args.action != 'compile', args.force)
                           for job in jobs]
                records = [future.result() for future in futures]
        save_report(records, args, run_dir, modules)
        if any(r['status'] == 'failed' for r in records):
            raise RuntimeError('One or more jobs failed; see report logs')


if __name__ == '__main__':
    try:
        main()
    except (RuntimeError, OSError, ValueError) as exc:
        print(str(exc), file=sys.stderr)
        sys.exit(1)
