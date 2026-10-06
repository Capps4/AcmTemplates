"""Run bounded correctness suites; retain evidence, never executables or logs."""
import concurrent.futures
import json
import os
from pathlib import Path
import re
import shlex
import sys
import tempfile
import time
import threading
from contextlib import nullcontext

import tools as config
from tools.Common import (ROOT, FLAGS, compiler, digest, execute, headers, manifest,
                          test_path, snapshot, ProcessError, cached_program, cache_program, include_digests, includes_current)


def module_jobs(name, profile):
    folder = test_path(name)
    sources = [folder / 'Test.cpp', *sorted(folder.glob('TestExtra*.cpp'))]
    # Demos are included by their module suite; only configuration/containment
    # checks that need an isolated translation unit keep separate entry points.
    groups = [('main', sources)] + [(p.stem, [p]) for p in sorted(folder.glob('TestCase*.cpp'))]
    inventory = [p for p in folder.rglob('*') if p.is_file() and '__pycache__' not in p.parts]
    return [{'id': f'{name}/{profile}/{label}', 'module': name, 'profile': profile,
             'sources': src, 'inputs': inventory, 'headers': headers(name)}
            for label, src in groups]


def signature(job, comp, seed):
    paths = [*job['sources'], *job['headers'], ROOT / 'tools/Run.py',
             ROOT / 'tools/Common.py', ROOT / 'tools/Catalog.py',
             ROOT / 'tools/__init__.py', ROOT / 'Manifest.json']
    case_catalog = ROOT / 'tests/Correctness/Cases.json'
    inventory = [*job['inputs'], *([case_catalog] if case_catalog.exists() else [])]
    return {'sha256': snapshot(paths, inventory), 'compiler': comp,
            'flags': FLAGS[job['profile']], 'seed': seed, 'profile': job['profile'],
            'case': config.TEST_CASE,
            'environment': {key: os.environ.get(key) for key in
                            ('CPATH', 'CPLUS_INCLUDE_PATH', 'LIBRARY_PATH', 'SDKROOT',
                             'MACOSX_DEPLOYMENT_TARGET')}}


def current(record, job=None, actual_digests=None):
    try:
        sig = record['signature']
        if record['module'] == 'Tools':
            return tools_signature() == sig
        job = job or next((j for j in module_jobs(record['module'], sig['profile'])
                           if j['id'] == record['id']), None)
        return (job is not None and not sig.get('case') and
                signature(job, compiler(sig['profile']), config.RIGHT_SEED) == sig and
                includes_current(record.get('actualIncludes', {}), actual_digests))
    except (KeyError, OSError, RuntimeError, ValueError):
        return False


def compile_job(job, comp, build, record, activity=None, compile_limit=None, actual_digests=None):
    cached = cached_program(job['id'], record['signature'], actual_digests)
    if cached:
        target, includes = cached
        record.update(compileMs=0, compileCached=True, actualIncludes=includes)
        if activity:
            activity(job['id'], '复用编译，重新运行')
        return target
    if activity:
        activity(job['id'], '等待编译')
    with compile_limit or nullcontext():
        folder = build / job['id']
        folder.mkdir(parents=True)
        objects, actual = [], set()
        start = time.monotonic()
        for i, source in enumerate(job['sources']):
            obj, dep = folder / f'{i}.o', folder / f'{i}.d'
            command = [comp['path'], *FLAGS[job['profile']], '-MD', '-MF', str(dep),
                       '-c', str(source), '-o', str(obj)]
            record.update(phase='compile', command=command)
            if activity:
                activity(job['id'], '编译 ' + source.name)
            execute(command, timeout=config.COMPILE_TIMEOUT)
            objects.append(str(obj))
            text = dep.read_text().replace('\\\n', ' ')
            for name in shlex.split(text.split(':', 1)[1]):
                path = Path(name).resolve()
                if path.is_file():
                    actual.add(path)
        target = folder / 'program'
        command = [comp['path'], *FLAGS[job['profile']], *objects, '-o', str(target)]
        record.update(phase='link', command=command)
        if activity:
            activity(job['id'], '链接')
        execute(command, timeout=config.COMPILE_TIMEOUT)
        record['compileMs'] = (time.monotonic() - start) * 1000
        record['actualIncludes'] = {str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p): digest(p) for p in sorted(actual)}
        target = cache_program(job['id'], record['signature'], target, record['actualIncludes'])
        record['compileCached'] = False
        return target


def failure(record, error):
    text = str(error)
    output = getattr(error, 'stdout', '')
    begun = re.findall(r'^CASE_BEGIN (.+)$', output, re.M)
    failed = re.findall(r'^CASE_FAIL ([^:]+):', output + '\n' + getattr(error, 'stderr', ''), re.M)
    if failed or begun:
        record['failedCase'] = (failed or begun)[-1]
    elif record.get('module') == 'Tools':
        match = re.search(r'^(?:FAIL|ERROR): (.+)$', text, re.M)
        if match:
            record['failedCase'] = match.group(1)
    record.update(status='failed', error=text[:32000])
    if isinstance(error, ProcessError):
        record.update(command=error.command, returncode=error.returncode,
                      timeout=error.timeout)
    return record


def verify_job(job, comp, seed, build, activity=None, compile_limit=None, actual_digests=None):
    record = {'id': job['id'], 'module': job['module'], 'profile': job['profile'],
              'status': 'failed', 'phase': 'dependencies', 'caseIds': []}
    try:
        if activity:
            activity(job['id'], '读取依赖')
        sig = signature(job, comp, seed)
        record['signature'] = sig
        target = compile_job(job, comp, build, record, activity, compile_limit, actual_digests)
        record.update(phase='run', command=[str(target)])
        if activity:
            activity(job['id'], '运行用例')
        start = time.monotonic()
        result = execute([str(target)], timeout=config.TEST_TIMEOUT,
                         env={'TEST_SEED': str(seed), 'TEST_CASE': config.TEST_CASE or ''})
        record['processMs'] = (time.monotonic() - start) * 1000
        record['caseIds'] = re.findall(r'^CASE_PASS (.+)$', result.stdout, re.M)
        durations = re.findall(r'^CASE_TIME (\S+) ([\d.eE+-]+)$', result.stdout, re.M)
        record['caseMs'] = {ident: float(ms) for ident, ms in durations}
        if record['caseIds'] and (set(record['caseIds']) != set(record['caseMs']) or
                any(not 0 <= ms < float('inf') for ms in record['caseMs'].values())):
            raise RuntimeError('Case timing evidence is incomplete or invalid')
        record['runMs'] = sum(record['caseMs'].values())
        if activity:
            activity(job['id'], '检查验证证据')
        if signature(job, comp, seed) != sig:
            raise RuntimeError('Inputs changed during verification; rerun')
        record.update(status='passed', phase='complete')
        record.pop('command', None)
    except Exception as exc:
        failure(record, exc)
    finally:
        if activity:
            activity(job['id'], None)
    return record


def tools_inputs():
    folder = ROOT / 'tests/Tools'
    if not list(folder.glob('*Test.py')):
        return []
    return [*folder.glob('*.py'), *(ROOT / 'tools').rglob('*.py'), ROOT / 'ctl',
            ROOT / 'Manifest.json', ROOT / 'Library.md', ROOT / 'tools/Library.json',
            ROOT / 'tools/RuleExceptions.json', ROOT / 'tools/requirements.txt',
            *config.SOURCE.rglob('*.hpp'), *(ROOT / 'tests/Support').glob('*.hpp')]


def tools_signature():
    return {'kind': 'tools', 'sha256': snapshot([], tools_inputs()), 'python': sys.version,
            'compilerProfiles': {p: compiler(p) for p in ('strict', 'optimized', 'sanitized')}}


def verify_tools(activity=None):
    record = {'id': 'Tools/right', 'module': 'Tools', 'status': 'failed', 'phase': 'tools'}
    try:
        if activity:
            activity(record['id'], '读取工具依赖')
        record['signature'] = tools_signature()
        command = [sys.executable, '-B', '-m', 'unittest', 'discover',
                   '-s', str(ROOT / 'tests/Tools'), '-p', '*Test.py']
        record['command'] = command
        if activity:
            activity(record['id'], '运行工具回归')
        start = time.monotonic()
        execute(command, timeout=max(180, config.TEST_TIMEOUT))
        record['runMs'] = (time.monotonic() - start) * 1000
        if activity:
            activity(record['id'], '检查验证证据')
        if tools_signature() != record['signature']:
            raise RuntimeError('Tool inputs changed during verification')
        record.update(status='passed', phase='complete')
        record.pop('command', None)
    except Exception as exc:
        failure(record, exc)
    finally:
        if activity:
            activity(record['id'], None)
    return record


def run(names=None, progress=None, activity=None):
    if min(config.TEST_JOBS, config.TEST_RUN_JOBS) < 1 or min(config.TEST_TIMEOUT, config.COMPILE_TIMEOUT) <= 0 or config.RIGHT_SEED < 0:
        raise ValueError('测试并发、种子或超时配置无效')
    if (not config.RIGHT_PROFILES or len(set(config.RIGHT_PROFILES)) != len(config.RIGHT_PROFILES) or
            set(config.RIGHT_PROFILES) - {'strict', 'optimized', 'sanitized'}):
        raise ValueError('未知正确性编译模式')
    modules = manifest()
    names = sorted(modules) if names is None else list(dict.fromkeys(names))
    if not names or set(names) - set(modules):
        raise ValueError('测试模块为空或含未知模块')
    with_tools = (set(names) == set(modules) and not config.TEST_CASE and
                  set(config.RIGHT_PROFILES) == {'strict', 'optimized', 'sanitized'} and tools_inputs())
    jobs = [job for name in names for profile in config.RIGHT_PROFILES for job in module_jobs(name, profile)]
    total = len(jobs) + int(bool(with_tools))
    if progress:
        progress(0, total, '正确性：准备编译器与测试记录', 'running')
    compilers = {p: compiler(p) for p in config.RIGHT_PROFILES}
    records = []
    pending_records = [{'id': j['id'], 'module': j['module'], 'profile': j['profile'],
                        'status': 'interrupted', 'phase': 'pending', 'error': '本轮未完成'} for j in jobs]
    if with_tools:
        pending_records.append({'id': 'Tools/right', 'module': 'Tools', 'status': 'interrupted',
                                'phase': 'pending', 'error': '本轮未完成'})
    from tools.Verification import save, read_evidence
    initial_digests = include_digests(read_evidence().get('right', []))
    # Invalidate older successes before any process starts, including interrupted runs.
    save(right=pending_records)
    config.CACHE.mkdir(parents=True, exist_ok=True)
    try:
        if with_tools:
            if progress:
                progress(0, total, '正确性：工具回归', 'running')
            records.append(verify_tools(activity))
        if progress:
            progress(len(records), total, '正确性：编译与运行', 'running')
        with tempfile.TemporaryDirectory(prefix='test-', dir=config.CACHE) as tmp:
            compile_limit = threading.BoundedSemaphore(config.TEST_JOBS)
            with concurrent.futures.ThreadPoolExecutor(max_workers=config.TEST_RUN_JOBS) as pool:
                pending = [pool.submit(verify_job, job, compilers[job['profile']], config.RIGHT_SEED, Path(tmp), activity, compile_limit, initial_digests)
                           for job in jobs]
                try:
                    for future in concurrent.futures.as_completed(pending):
                        record = future.result()
                        records.append(record)
                        if progress:
                            detail = '最近完成 ' + record['id'] + '：' + ('通过' if record['status'] == 'passed' else '失败')
                            progress(len(records), total, '正确性：编译与运行', 'running', detail)
                finally:
                    for future in pending:
                        future.cancel()
    except BaseException:
        done = {r['id'] for r in records}
        save(right=[*records, *[r for r in pending_records if r['id'] not in done]])
        raise
    if progress:
        progress(len(records), total, '正确性：检查覆盖并保存报告', 'running')
    final_digests = include_digests(records)
    for record in records:
        if record['status'] == 'passed' and not includes_current(record.get('actualIncludes', {}), final_digests):
            failure(record, RuntimeError('Compiler inputs changed during verification; rerun'))
    if config.TEST_CASE and not any(r.get('caseIds') for r in records):
        records.append({'id': 'Selection/right', 'module': 'Selection', 'status': 'failed',
                        'phase': 'selection', 'error': '未找到 case：' + config.TEST_CASE})
    if not config.TEST_CASE:
        try:
            cases = json.loads((ROOT / 'tests/Correctness/Cases.json').read_text())['addedCases']
            for name in names:
                required = {c['id'] for c in cases if c['module'] == name}
                for profile in config.RIGHT_PROFILES:
                    group = [r for r in records if r['module'] == name and r.get('profile') == profile]
                    if any(r['status'] != 'passed' for r in group):
                        continue  # Preserve the original compile/run failure, without a duplicate coverage error.
                    seen = {ident for r in group for ident in r.get('caseIds', [])}
                    if not required or not required.issubset(seen):
                        records.append({'id': name + '/' + profile + '/coverage', 'module': name,
                                        'profile': profile, 'status': 'failed', 'phase': 'coverage',
                                        'error': '缺少具名 case 输出：' + ', '.join(sorted(required - seen))
                                                 if required else '模块缺少覆盖清单'})
        except (KeyError, OSError, TypeError, ValueError) as exc:
            records.append({'id': 'Selection/right', 'module': 'Selection', 'status': 'failed',
                            'phase': 'coverage', 'error': '覆盖清单无效：' + str(exc)})
    _, path = save(right=records)
    return {'records': records, 'path': str(path), 'passed': all(r['status'] == 'passed' for r in records)}
