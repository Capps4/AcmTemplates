"""Reproducible GNU++17 correctness checks. See README.md for the workflow."""
import concurrent.futures
import datetime
import fcntl
import json
import os
import platform
import sys
import uuid
from types import SimpleNamespace

import tools as config
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.Common import ROOT, FLAGS, compiler, hardware, digest, execute, fingerprint, headers, manifest, module_path, test_path, write_json

REPORTS = config.REPORTS
BUILD = config.CACHE / 'build'
STATE = REPORTS / 'State.json'


def load_state():
    return json.loads(STATE.read_text()) if STATE.exists() else {}


def signature(job, comp, seed):
    return {'sha256': fingerprint(job['inputs'] + [ROOT / 'tools/Run.py', ROOT / 'tools/Common.py', ROOT / 'tools/Catalog.py', ROOT / 'tools/__init__.py', ROOT / 'Manifest.json']),
            'compiler': comp, 'flags': FLAGS[job['profile']], 'cases': [list(c) for c in job['cases']],
            'sources': [str(p.relative_to(ROOT)) for p in job['sources']],
            'seed': seed, 'profile': job['profile'], 'kind': job.get('kind', 'module'),
            'environment': {key: os.environ.get(key) for key in
                            ('CPATH', 'CPLUS_INCLUDE_PATH', 'LIBRARY_PATH', 'SDKROOT', 'MACOSX_DEPLOYMENT_TARGET')}}


def current(record):
    try:
        sig = record['signature']
        if sig.get('kind') == 'tools':
            return tools_signature() == sig
        paths = [ROOT / name for name in sig['sha256']]
        if sig.get('kind', 'module') == 'module' and record['module'] != 'Integration':
            job = next((j for j in module_jobs(record['module'], sig['profile']) if j['id'] == record['id']), None)
            if not job or [str(p.relative_to(ROOT)) for p in job['sources']] != sig['sources']:
                return False
        return (fingerprint(paths) == sig['sha256'] and
                compiler(sig['profile']) == sig['compiler'] and
                FLAGS[sig['profile']] == sig['flags'] and
                all(os.environ.get(k) == v for k, v in sig.get('environment', {}).items()) and
                all(digest(ROOT / p) == val for p, val in record.get('actualIncludes', {}).items()))
    except (KeyError, OSError, RuntimeError, ValueError):
        return False


def module_jobs(name, profile):
    folder = test_path(name)
    sources = [folder / 'Test.cpp', *sorted(folder.glob('TestExtra*.cpp'))]
    cases = [*folder.glob('TestCase*.cpp'), *folder.glob('*ExamplesTest.cpp')]
    # Trie's Test.cpp already executes its three demos with shared query helpers.
    if name != 'Trie':
        cases.extend(folder.glob('Demo*.cpp'))
    groups = [('main', sources)] + [(p.stem, [p]) for p in sorted(cases)]
    jobs = [{'id': f'{name}/{profile}/{label}', 'module': name, 'profile': profile,
             'sources': src, 'inputs': src + headers(name), 'cases': [(None, None)]}
            for label, src in groups]
    if name == 'Geo2':
        for job in jobs:
            if job['sources'][0].name == 'TestCaseHalfPlane.cpp':
                job['inputs'].append(folder / 'HalfPlaneRegression.txt')
        for label in ('Layer1', 'Layer2', 'Layer3', 'HalfPlaneGenerated'):
            source = BUILD / 'Sources' / f'Geo2{label}.cpp'
            extra = [folder / 'GenerateHalfPlaneCases.py', BUILD / 'Geo2/HalfPlaneGenerated.txt'] if label == 'HalfPlaneGenerated' else []
            jobs.append({'id': f'Geo2/{profile}/{label}', 'module': name, 'profile': profile,
                         'sources': [source], 'inputs': [source, *headers(name), *extra],
                         'cases': [(None, None)]})
    if name == 'Geo2':
        sources = [BUILD / 'Sources' / f'Geo2{label}.cpp'
                   for label in ('MultiTU', 'PointWitness', 'CircleWitness')]
        jobs.append({'id': f'Geo2/{profile}/MultiTU', 'module': name, 'profile': profile,
                     'sources': sources, 'inputs': sources + headers(name), 'cases': [(None, None)]})
    return jobs


def prepare_geo2(seed):
    """Generate exact Fraction oracles once; planning/cache checks do not run it."""
    folder = test_path('Geo2')
    dest = BUILD / 'Sources'
    dest.mkdir(parents=True, exist_ok=True)
    for layer in (1, 2, 3):
        source = dest / f'Geo2Layer{layer}.cpp'
        text = f'#define GEO2_TEST_LAYER {layer}\n#include "{folder / "TestCaseModules.cpp"}"\n'
        if not source.exists() or source.read_text() != text:
            source.write_text(text)
    shared = {
        'MultiTU': f'#define GEO2_TEST_MULTITU\n#include "{folder / "TestCaseModules.cpp"}"\n',
        'PointWitness': f'#include "{module_path("Geo2") / "PointVec.hpp"}"\n'
                        'int geometryPointWitness() { return orient(Point<long long>{0,0}, Point<long long>{1,0}, Point<long long>{0,1}); }\n',
        'CircleWitness': f'#include "{module_path("Geo2") / "Circle.hpp"}"\n'
                         'int geometryCircleWitness() { return convexHull<long long>({{0,0},{2,0},{2,2},{0,2}}).size(); }\n',
    }
    for label, text in shared.items():
        path = dest / f'Geo2{label}.cpp'
        if not path.exists() or path.read_text() != text:
            path.write_text(text)
    cases = BUILD / 'Geo2/HalfPlaneGenerated.txt'
    cases.parent.mkdir(parents=True, exist_ok=True)
    execute([sys.executable, folder / 'GenerateHalfPlaneCases.py', cases,
             '--seed', str(seed), '--rounds', '1200'])
    source = dest / 'Geo2HalfPlaneGenerated.cpp'
    header, file = json.dumps(str(folder / 'TestCaseHalfPlane.cpp')), json.dumps(str(cases))
    text = (f'#define main halfPlaneMain\n#include {header}\n#undef main\n'
            f'int main() {{ char name[] = "hpi"; char file[] = {file}; '
            'char *argv[] = {name, file}; return halfPlaneMain(2, argv); }\n')
    if not source.exists() or source.read_text() != text:
        source.write_text(text)


def integration_jobs(profile):
    from tools.testing.Integration import inputs, assemble
    from tools.testing.Options import MODULES, source
    modules = inputs()
    folder = BUILD / 'Sources'
    folder.mkdir(parents=True, exist_ok=True)
    all_headers = [module_path(name, modules) / ('Circle.hpp' if name == 'Geo2' else 'code.hpp') for name in modules]
    common = [ROOT / 'tools/testing/Integration.py', ROOT / 'Manifest.json', *all_headers]
    jobs = []
    for name, text, cases, paths in [
        ('library', assemble(modules), [(None, None)], [ROOT / 'tests/Correctness/Integration/Test.hpp']),
        ('options', source(), [(None, None)], [ROOT / 'tools/testing/Options.py',
                                              *[module_path(name) / 'code.hpp' for name in MODULES]])
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


def verify_job(job, comp, seed, previous, run_dir, timeout, force):
    record = {'id': job['id'], 'module': job['module'], 'cached': False,
              'status': 'failed', 'commands': [], 'executions': []}
    log = run_dir / (job['id'].replace('/', '-') + '.log')
    record['log'] = str(log.relative_to(ROOT))
    try:
        sig = signature(job, comp, seed)
        record['signature'] = sig
        old = previous.get(job['id'], {})
        target = BUILD / job['id'] / 'program'
        reuse = (old.get('status') == 'passed' and old.get('signature') == sig
                 and target.exists() and current(old))
        if reuse and not force:
            return dict(old, cached=True)
        target, commands, actual, diagnostics = compile_job(job, comp, config.COMPILE_TIMEOUT)
        record['commands'] = commands
        record['actualIncludes'] = {str(p.relative_to(ROOT)): digest(p) for p in actual}
        log.write_text(diagnostics)
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
        record['status'] = 'passed'
    except Exception as exc:
        record['error'] = str(exc)
        with log.open('a') as out:
            out.write('\n' + str(exc))
    return record


def summary_text(report):
    lines = ['# 验证报告', '', f"运行：{report['runId']}；平台：{report['platform']}。", '',
             '| 项目 | 状态 | 缓存 |', '| --- | --- | --- |']
    for record in report['records']:
        lines.append(f"| {record['id']} | {record['status']} | {'是' if record.get('cached') else '否'} |")
        if record.get('error'):
            lines.extend(['', '```text', record['error'], '```', ''])
    return '\n'.join(lines) + '\n'


def save_report(records, args, run_dir):
    report = {'formatVersion': 3, 'runId': run_dir.name, 'platform': platform.platform(),
              'hardware': hardware(), 'command': args.command, 'seed': args.seed,
              'records': records}
    write_json(run_dir / 'Results.json', report)
    (run_dir / 'Summary.md').write_text(summary_text(report))
    write_json(REPORTS / 'Latest.json', {'report': str((run_dir / 'Results.json').relative_to(ROOT))})
    state = load_state()
    for record in records:
        state[record['id']] = record
    write_json(STATE, state)
    return report


def tools_inputs():
    folder = ROOT / 'tests/Tools'
    if not list(folder.glob('*Test.py')):
        return []
    paths = [*folder.glob('*.py'), *(ROOT / 'tools').rglob('*.py'),
             ROOT / 'ctl', ROOT / 'Manifest.json', ROOT / 'Library.md',
             ROOT / 'tools/Library.json', ROOT / 'tools/RuleExceptions.json',
             ROOT / 'tools/requirements.txt', ROOT / 'tests/Support/TestSupport.hpp']
    paths.extend(config.SOURCE.rglob('*.hpp'))
    return paths


def tools_signature():
    return {'kind': 'tools', 'sha256': fingerprint(tools_inputs()),
            'python': sys.version, 'compilerProfiles': {p: compiler(p) for p in config.RIGHT_PROFILES}}


def verify_tools(previous, run_dir):
    record = {'id': 'Tools/right', 'module': 'Tools', 'cached': False,
              'status': 'failed', 'commands': [], 'executions': []}
    log = run_dir / 'Tools-right.log'
    record['log'] = str(log.relative_to(ROOT))
    try:
        record['signature'] = tools_signature()
        old = previous.get(record['id'], {})
        if not config.FORCE_RIGHT and old.get('status') == 'passed' and old.get('signature') == record['signature']:
            return dict(old, cached=True)
        command = [sys.executable, '-B', '-m', 'unittest', 'discover',
                   '-s', str(ROOT / 'tests/Tools'), '-p', '*Test.py']
        record['commands'] = [command]
        result = execute(command, timeout=max(180, config.TEST_TIMEOUT))
        log.write_text(result.stdout + result.stderr)
        record['executions'] = [{'output': result.stdout, 'stderr': result.stderr}]
        if tools_signature() != record['signature']:
            raise RuntimeError('Tool inputs changed during verification')
        record['status'] = 'passed'
    except Exception as exc:
        record['error'] = str(exc)
        log.write_text(str(exc))
    return record


def run(names=None, progress=None):
    """Run correctness profiles; configuration and public arguments live in CTL."""
    if config.TEST_JOBS < 1 or config.TEST_TIMEOUT <= 0 or config.COMPILE_TIMEOUT <= 0 or config.RIGHT_SEED < 0:
        raise ValueError('tools/__init__.py 中 TEST_JOBS、RIGHT_SEED 或超时配置无效')
    if set(config.RIGHT_PROFILES) != {'strict', 'optimized', 'sanitized'}:
        raise ValueError('RIGHT_PROFILES 必须包含 strict、optimized、sanitized')
    REPORTS.mkdir(parents=True, exist_ok=True)
    with (REPORTS / '.run.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        modules, previous = manifest(), load_state()
        names = sorted(modules) if names is None else list(dict.fromkeys(names))
        if not names or set(names) - set(modules):
            raise ValueError('TEST_MODULES 为空或含未知模块')
        args = SimpleNamespace(seed=config.RIGHT_SEED, command=['ctl', 'test', '--right'])
        if 'Geo2' in names:
            prepare_geo2(args.seed)
        jobs = [job for name in names for profile in config.RIGHT_PROFILES
                for job in module_jobs(name, profile)]
        if set(names) == set(modules):
            jobs.extend(job for profile in config.RIGHT_PROFILES for job in integration_jobs(profile))
        run_id = datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + uuid.uuid4().hex[:8]
        run_dir = REPORTS / run_id
        run_dir.mkdir()
        compilers = {p: compiler(p) for p in config.RIGHT_PROFILES}
        records = []
        with_tools = set(names) == set(modules) and bool(tools_inputs())
        total = len(jobs) + int(with_tools)
        with concurrent.futures.ThreadPoolExecutor(max_workers=config.TEST_JOBS) as pool:
            futures = [pool.submit(verify_job, job, compilers[job['profile']], args.seed,
                                   previous, run_dir, config.TEST_TIMEOUT, config.FORCE_RIGHT)
                       for job in jobs]
            for future in concurrent.futures.as_completed(futures):
                record = future.result()
                records.append(record)
                if progress:
                    progress(len(records), total, record['id'],
                             'cached' if record.get('cached') else record['status'],
                             '日志：' + str(ROOT / record['log']) if record['status'] == 'failed' else None)
        if with_tools:
            record = verify_tools(previous, run_dir)
            records.append(record)
            if progress:
                progress(len(records), total, record['id'],
                         'cached' if record.get('cached') else record['status'],
                         '日志：' + str(ROOT / record['log']) if record['status'] == 'failed' else None)
        records.sort(key=lambda record: record['id'])
        result = save_report(records, args, run_dir)
        result['path'] = str(run_dir / 'Summary.md')
        result['passed'] = all(r['status'] == 'passed' for r in records)
        return result
