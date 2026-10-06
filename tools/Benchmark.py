"""Parallel benchmark compilation followed by isolated, sequential measurement."""
import concurrent.futures
import json
import os
import shlex
from contextlib import ExitStack
from pathlib import Path
import subprocess
import sys
import tempfile
import time

import tools as config
from tools.Common import (ROOT, FLAGS, compiler, execute, snapshot, headers, manifest,
                          benchmark_path, ProcessError, hardware, digest, cached_program, cache_program)
from tools.Run import failure


def workload_sizes(name):
    path = benchmark_path(name) / 'Settings.json'
    data = json.loads(path.read_text(encoding='utf-8')) if path.exists() else {}
    if not isinstance(data, dict):
        raise ValueError('性能配置须为 JSON 对象：' + str(path))
    sizes = data.get('sizes', config.PERF_DEFAULT_SIZES)
    if (not isinstance(sizes, (list, tuple)) or len(sizes) != 2 or
            any(type(n) is not int or n <= 0 for n in sizes) or sizes[0] >= sizes[1]):
        raise ValueError('性能规模须为两个递增的正整数：' + str(path))
    return tuple(sizes)


def workload_iterations(name, source):
    path = benchmark_path(name) / 'Settings.json'
    data = json.loads(path.read_text(encoding='utf-8')) if path.exists() else {}
    value = data.get('iterations', 1)
    if isinstance(value, dict):
        value = value.get(source.stem, 1)
    if type(value) is not int or value < 1:
        raise ValueError('性能批次须为正整数：' + str(path))
    return value


def signature(name, source):
    paths = [source, *headers(name), ROOT / 'tools/Benchmark.py', ROOT / 'tools/Run.py',
             ROOT / 'tools/Common.py', ROOT / 'tools/Catalog.py',
             ROOT / 'tools/__init__.py', ROOT / 'Manifest.json']
    inventory = [p for p in source.parent.rglob('*') if p.is_file()]
    return {'sha256': snapshot(paths, inventory), 'compiler': compiler('optimized'),
            'flags': FLAGS['optimized'], 'sizes': list(workload_sizes(name)),
            'seed': config.PERF_SEED, 'repeats': config.PERF_REPEATS,
            'iterations': workload_iterations(name, source), 'hardware': hardware(), 'mode': 'batched-v1',
            'environment': {key: os.environ.get(key) for key in
                            ('CPATH', 'CPLUS_INCLUDE_PATH', 'LIBRARY_PATH', 'SDKROOT',
                             'MACOSX_DEPLOYMENT_TARGET')}}


def skipped_record(name, source):
    path = benchmark_path(name) / 'Settings.json'
    data = json.loads(path.read_text(encoding='utf-8')) if path.exists() else {}
    if not isinstance(data, dict):
        raise ValueError('性能配置须为 JSON 对象：' + str(path))
    if 'skip' not in data:
        return None
    reason = data['skip']
    if not isinstance(reason, str) or not reason.strip():
        raise ValueError('性能跳过须提供非空原因：' + str(path))
    if source.parent != benchmark_path(name) or not source.name.startswith('Benchmark') or source.suffix != '.cpp':
        raise ValueError('性能占位入口无效：' + str(source))
    paths = [source, *headers(name), ROOT / 'tools/Benchmark.py', ROOT / 'tools/Run.py',
             ROOT / 'tools/Common.py', ROOT / 'tools/Catalog.py',
             ROOT / 'tools/__init__.py', ROOT / 'Manifest.json']
    inventory = [p for p in source.parent.rglob('*') if p.is_file()]
    return {'id': name + '/' + source.stem, 'module': name,
            'source': str(source.relative_to(ROOT)), 'status': 'skipped',
            'phase': 'skipped', 'reason': reason, 'samples': [],
            'signature': {'sha256': snapshot(paths, inventory), 'kind': 'performance-skip'}}


def current(record):
    try:
        if record['status'] == 'skipped':
            return record == skipped_record(record['module'], ROOT / record['source'])
        if record['status'] != 'passed':
            return False
        source = ROOT / record['source']
        if record['id'] != record['module'] + '/' + source.stem or signature(record['module'], source) != record['signature']:
            return False
        if any(digest(ROOT / p) != value for p, value in record.get('actualIncludes', {}).items()):
            return False
        samples = record['samples']
        expected = {(n, shape) for n in workload_sizes(record['module']) for shape in (0, 1)}
        return (len(samples) == len(expected) and
                {(s['n'], s['shape']) for s in samples} == expected and
                all(type(s['warmupMs']) in (float, int) and 0 <= s['warmupMs'] < float('inf') and
                    s['seed'] == config.PERF_SEED and s['iterations'] == record['signature']['iterations'] and
                    type(s['iterations']) is int and len(s['samplesMs']) == config.PERF_REPEATS and
                    all(type(ms) in (float, int) and 0 <= ms < float('inf') for ms in s['samplesMs'])
                    for s in samples))
    except (KeyError, OSError, RuntimeError, ValueError, TypeError):
        return False


def stop_process(prepared):
    if prepared is None:
        return
    process = prepared['process']
    if process.returncode is None:
        process.kill()
        process.wait()
    if process.stdin and not process.stdin.closed:
        try:
            process.stdin.close()
        except BrokenPipeError:
            pass
    prepared['out'].close()
    prepared['err'].close()


def prepare_process(command):
    out, err = tempfile.TemporaryFile(mode='w+'), tempfile.NamedTemporaryFile(mode='w+')
    prepared = {'out': out, 'err': err, 'process': None}
    try:
        process = subprocess.Popen([*command, '--wait'], stdin=subprocess.PIPE, stdout=out, stderr=err, text=True)
        prepared['process'] = process
        deadline = time.monotonic() + config.TEST_TIMEOUT
        while True:
            # Separate reader: do not seek the writer's shared file descriptor.
            output = Path(err.name).read_text()
            if 'CTL_BENCHMARK_READY\n' in output:
                return prepared
            code = process.poll()
            if code is not None:
                out.seek(0)
                raise ProcessError(command, out.read(), output, code)
            if time.monotonic() >= deadline:
                raise ProcessError(command, '', output, timeout=config.TEST_TIMEOUT)
            time.sleep(.005)
    except BaseException:
        if prepared['process'] is not None:
            stop_process(prepared)
        else:
            out.close(); err.close()
        raise


def measure_process(command, timeout=None, prepared=None):
    timeout = config.TEST_TIMEOUT if timeout is None else timeout
    with ExitStack() as stack:
        if prepared is None:
            out = stack.enter_context(tempfile.TemporaryFile(mode='w+'))
            err = stack.enter_context(tempfile.TemporaryFile(mode='w+'))
            process = subprocess.Popen(command, stdin=subprocess.DEVNULL, stdout=out, stderr=err)
        else:
            out, err, process = prepared['out'], prepared['err'], prepared['process']
            process.stdin.write('\n')
            process.stdin.flush()
            process.stdin.close()
        deadline = time.monotonic() + timeout
        timed_out = False
        try:
            while True:
                pid, status, usage = os.wait4(process.pid, os.WNOHANG)
                if pid:
                    process.returncode = os.waitstatus_to_exitcode(status)
                    break
                if time.monotonic() > deadline:
                    process.kill()
                    _, status, usage = os.wait4(process.pid, 0)
                    process.returncode = os.waitstatus_to_exitcode(status)
                    timed_out = True
                    break
                time.sleep(.001)
        except BaseException:
            process.kill()
            os.wait4(process.pid, 0)
            process.returncode = -9
            raise
        out.seek(0)
        err.seek(0)
        stdout, stderr = out.read(), err.read()
        if timed_out or process.returncode:
            raise ProcessError(command, stdout, stderr, process.returncode,
                               timeout if timed_out else None)
        try:
            samples = [json.loads(line) for line in stdout.splitlines() if line.strip()]
        except ValueError as exc:
            raise RuntimeError('Invalid benchmark output: ' + stdout + '\n' + stderr) from exc
        return {'samples': samples,
                'peakRssBytes': int(usage.ru_maxrss * (1 if sys.platform == 'darwin' else 1024))}


def compile_one(name, source, folder, activity=None):
    label = name + '/' + source.stem
    target = folder / label / 'program'
    target.parent.mkdir(parents=True)
    record = {'id': label, 'module': name, 'source': str(source.relative_to(ROOT)),
              'status': 'failed', 'phase': 'dependencies', 'samples': []}
    try:
        if activity:
            activity(label, '读取性能依赖')
        sig = signature(name, source)
        record['signature'] = sig
        cached = cached_program('perf/' + label, sig)
        if cached:
            record.update(status='compiled', compileMs=0, compileCached=True, actualIncludes=cached[1])
            return record, cached[0]
        wrapper = target.parent / 'Batch.cpp'
        wrapper.write_text('#define main benchmark_entry\n#include ' + json.dumps(str(source)) + '\n#undef main\n#undef cin\n' + '''int main(int argc, char **argv) {
    if (argc != 4 and argc != 5) return 2;
    if (argc == 5) {
        std::cerr << "CTL_BENCHMARK_READY" << std::endl;
        if (std::cin.get() != '\\n') return 2;
    }
    char zero[] = "0", one[] = "1";
    for (int i : {1, 2}) {
        for (char *shape : {zero, one}) {
            char *args[] = {argv[0], argv[i], shape, argv[3], nullptr};
            std::cerr << "BENCHMARK_BEGIN n=" << argv[i] << " shape=" << shape << std::endl;
            int result = benchmark_entry(4, args);
            if (result) return result;
        }
    }
    return 0;
}
''', encoding='utf-8')
        command = [sig['compiler']['path'], *sig['flags'],
                   '-DBENCHMARK_REPEATS=' + str(config.PERF_REPEATS),
                   '-DBENCHMARK_ITERATIONS=' + str(sig['iterations']), '-MD', '-MF', str(target.parent / 'Batch.d'),
                   str(wrapper), '-o', str(target)]
        record.update(phase='compile', command=command)
        if activity:
            activity(label, '编译 ' + source.name)
        start = time.monotonic()
        execute(command, timeout=config.COMPILE_TIMEOUT)
        record.update(status='compiled', compileMs=(time.monotonic() - start) * 1000, compileCached=False)
        text = (target.parent / 'Batch.d').read_text().replace('\\\n', ' ')
        included = {Path(p).resolve() for p in shlex.split(text.split(':', 1)[1])}
        record['actualIncludes'] = {str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p): digest(p)
                                   for p in included if p.is_file() and p != wrapper}
        target = cache_program('perf/' + label, sig, target, record['actualIncludes'])
        record.pop('command', None)
    except Exception as exc:
        failure(record, exc)
    finally:
        if activity:
            activity(label, None)
    return record, target


def run(names=None, progress=None, activity=None):
    if min(config.TEST_TIMEOUT, config.COMPILE_TIMEOUT) <= 0 or config.TEST_JOBS < 1 or config.PERF_REPEATS < 3 or config.PERF_REPEATS % 2 != 1 or config.PERF_SEED < 0:
        raise ValueError('性能并发、种子或奇数采样次数配置无效')
    modules = manifest()
    names = sorted(modules) if names is None else list(dict.fromkeys(names))
    if not names or set(names) - set(modules):
        raise ValueError('测试模块为空或含未知模块')
    jobs = [(name, p) for name in names for p in sorted(benchmark_path(name).glob('Benchmark*.cpp'))]
    missing = set(names) - {name for name, _ in jobs}
    missing_records = [{'id': name + '/Benchmark', 'module': name, 'status': 'failed',
                        'phase': 'discovery', 'error': '模块缺少 Benchmark*.cpp'} for name in sorted(missing)]
    skipped = [record for name, source in jobs if (record := skipped_record(name, source)) is not None]
    skipped_ids = {record['id'] for record in skipped}
    jobs = [(name, source) for name, source in jobs if name + '/' + source.stem not in skipped_ids]
    config.CACHE.mkdir(parents=True, exist_ok=True)
    from tools.Verification import save
    placeholders = [{'id': name + '/' + source.stem, 'module': name, 'status': 'interrupted',
                     'phase': 'pending', 'error': '本轮未完成'} for name, source in jobs]
    placeholders.extend([*missing_records, *skipped])
    if progress:
        progress(0, len(jobs), '性能：准备测试记录', 'running')
    save(perf=placeholders)
    compiled = []
    prepared = {}
    try:
        with tempfile.TemporaryDirectory(prefix='test-perf-', dir=config.CACHE) as tmp:
            if progress:
                progress(0, len(jobs), '性能：编译', 'running')
            with concurrent.futures.ThreadPoolExecutor(max_workers=config.TEST_JOBS) as pool:
                pending = [pool.submit(compile_one, name, source, Path(tmp), **({'activity': activity} if activity else {}))
                           for name, source in jobs]
                try:
                    for future in concurrent.futures.as_completed(pending):
                        compiled.append(future.result())
                        if progress:
                            progress(len(compiled), len(jobs), '性能：编译', 'running')
                finally:
                    for future in pending:
                        future.cancel()
            if progress:
                progress(0, len(compiled), '性能：准备进程（等待测量）', 'running')
            def prepare_one(record, target):
                if record['status'] != 'compiled':
                    return
                try:
                    record.update(phase='startup')
                    command = [str(target), *map(str, record['signature']['sizes']), str(config.PERF_SEED)]
                    if activity:
                        activity(record['id'], '启动进程并等待')
                    prepared[record['id']] = prepare_process(command)
                except Exception as exc:
                    failure(record, exc)
                finally:
                    if activity:
                        activity(record['id'], None)
            with concurrent.futures.ThreadPoolExecutor(max_workers=config.TEST_JOBS) as pool:
                pending = [pool.submit(prepare_one, record, target) for record, target in compiled]
                for done, future in enumerate(concurrent.futures.as_completed(pending), 1):
                    future.result()
                    if progress:
                        progress(done, len(compiled), '性能：准备进程（等待测量）', 'running')
            # All workers have finished startup and now block on stdin.
            # No compiler, process startup or other workload shares the timed phase.
            if progress:
                progress(0, len(compiled), '性能：测量', 'running')
            for done, (record, target) in enumerate(sorted(compiled, key=lambda item: item[0]['id']), 1):
                if record['status'] == 'compiled':
                    try:
                        command = [str(target), *map(str, record['signature']['sizes']), str(config.PERF_SEED)]
                        record.update(phase='measure', command=command,
                                      workload={'sizes': record['signature']['sizes'], 'shapes': [0, 1]})
                        if activity:
                            activity(record['id'], '顺序测量四组负载')
                        start = time.monotonic()
                        measured = measure_process(command, prepared=prepared.get(record['id']))
                        record.update(samples=measured['samples'], processMs=(time.monotonic() - start) * 1000,
                                      batchPeakRssBytes=measured.get('peakRssBytes'))
                        if activity:
                            activity(record['id'], '检查测量证据')
                        if signature(record['module'], ROOT / record['source']) != record['signature']:
                            raise RuntimeError('Benchmark inputs changed; repeat measurements')
                        record.update(status='passed', phase='complete')
                        if not current(record):
                            raise RuntimeError('Benchmark returned invalid timing evidence')
                        record.pop('command', None)
                    except Exception as exc:
                        failure(record, exc)
                    finally:
                        if activity:
                            activity(record['id'], None)
                if progress:
                    detail = '最近完成 ' + record['id'] + '：' + ('通过' if record['status'] == 'passed' else '失败')
                    progress(done, len(compiled), '性能：测量', 'running', detail)
    except BaseException:
        records = [*missing_records, *skipped, *[record for record, _ in compiled]]
        for record in records:
            if record['status'] == 'compiled':
                record.update(status='interrupted', error='本轮测量未完成')
        done = {r['id'] for r in records}
        save(perf=[*records, *[r for r in placeholders if r['id'] not in done]])
        raise
    finally:
        for process in prepared.values():
            stop_process(process)
    records = [*missing_records, *skipped, *[record for record, _ in compiled]]
    if progress:
        progress(len(compiled), len(compiled), '性能：保存报告', 'running')
    _, path = save(perf=records)
    return {'records': records, 'path': str(path), 'passed': all(r['status'] in ('passed', 'skipped') for r in records)}
