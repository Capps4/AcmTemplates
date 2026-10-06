"""Compile reproducible benchmarks, then time each workload sequentially."""
import concurrent.futures
import datetime
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time
import uuid
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from tools.Common import ROOT,FLAGS,compiler,dependencies,execute,fingerprint,hardware,manifest,benchmark_path,write_json

import tools as config

def measure_process(command, timeout=None):
    timeout = config.TEST_TIMEOUT if timeout is None else timeout
    # wait4 returns this child's own peak RSS, unlike cumulative RUSAGE_CHILDREN.
    # File-backed output avoids pipe deadlocks; /usr/bin/time -l requires a
    # macOS sysctl forbidden in sandboxed execution and is unnecessary here.
    with tempfile.TemporaryFile(mode='w+') as out, tempfile.TemporaryFile(mode='w+') as err:
        process = subprocess.Popen(command, stdout=out, stderr=err)
        deadline = time.monotonic() + timeout
        while True:
            pid, status, usage = os.wait4(process.pid, os.WNOHANG)
            if pid:
                process.returncode = os.waitstatus_to_exitcode(status)
                break
            if time.monotonic() > deadline:
                process.kill()
                os.wait4(process.pid, 0)
                process.returncode = -9
                raise RuntimeError('Benchmark timeout')
            time.sleep(.01)
        out.seek(0); err.seek(0)
        stdout, stderr = out.read(), err.read()
        if process.returncode:
            raise RuntimeError(f'Benchmark exit {process.returncode}: ' + stdout + stderr)
        sample = json.loads(stdout)
        sample['peakRssBytes'] = int(usage.ru_maxrss * (1 if sys.platform == 'darwin' else 1024))
        return sample, stdout + stderr

def compile_one(name,source,run_dir):
    label=name+'/'+source.stem
    target=config.CACHE/'benchmarks'/label/'program'
    target.parent.mkdir(parents=True,exist_ok=True)
    paths=dependencies([source,ROOT/'tools/Benchmark.py',ROOT/'tools/Common.py',ROOT/'tools/Catalog.py',ROOT/'tools/__init__.py',ROOT/'Manifest.json'])
    sig={'sha256':fingerprint(paths),'compiler':compiler('optimized'),'flags':FLAGS['optimized']}
    signature=target.with_suffix('.json')
    record={'id':label,'module':name,'source':str(source.relative_to(ROOT)),'signature':sig,'status':'failed','samples':[]}
    log=run_dir/(label.replace('/','-')+'.log')
    record['log']=str(log.relative_to(ROOT))
    try:
        if not target.exists() or not signature.exists() or json.loads(signature.read_text())!=sig:
            cmd=[sig['compiler']['path'],*sig['flags'],str(source),'-o',str(target)]
            record['command']=cmd
            result=execute(cmd,timeout=config.COMPILE_TIMEOUT)
            log.write_text(result.stdout+result.stderr)
            write_json(signature,sig)
        else:
            log.write_text('Matching compiler/source fingerprint; executable reused.\n')
        record['status']='compiled'
    except Exception as exc:
        record['error']=str(exc);log.write_text(str(exc))
    return record,target,log

def run(names=None, progress=None):
    modules = manifest()
    if config.TEST_JOBS < 1 or config.PERF_SEED < 0 or config.TEST_TIMEOUT <= 0 or config.COMPILE_TIMEOUT <= 0:
        raise ValueError('tools/__init__.py 中 TEST_JOBS、PERF_SEED 或超时配置无效')
    names = sorted(modules) if names is None else list(dict.fromkeys(names))
    if not names or set(names) - set(modules):
        raise ValueError('TEST_MODULES 为空或含未知模块')
    folder=config.REPORTS/(datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')+'-benchmark-'+uuid.uuid4().hex[:8])
    folder.mkdir(parents=True)
    missing=[name for name in names if not list(benchmark_path(name).glob('Benchmark*.cpp'))]
    if missing:raise ValueError('Missing benchmarks: '+', '.join(missing))
    jobs=[(name,p) for name in names for p in sorted(benchmark_path(name).glob('Benchmark*.cpp'))]
    compiled = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=config.TEST_JOBS) as pool:
        futures = [pool.submit(compile_one, *job, folder) for job in jobs]
        for future in concurrent.futures.as_completed(futures):
            item = future.result()
            compiled.append(item)
            if progress:
                record = item[0]
                progress(len(compiled), len(jobs), '编译 ' + record['id'], record['status'],
                         '日志：' + str(ROOT / record['log']) if record['status'] == 'failed' else None)
    compiled.sort(key=lambda item: item[0]['id'])
    for measured, (record, target, log) in enumerate(compiled, 1):
        if record['status'] != 'compiled':
            if progress:
                progress(measured, len(compiled), '测量 ' + record['id'], 'failed', '编译失败，未测量')
            continue
        try:
            for n in config.PERF_SIZES.get(record['module'],config.PERF_DEFAULT_SIZES):
                for shape in (0,1):
                    cmd=[str(target),str(n),str(shape),str(config.PERF_SEED)]
                    sample,output=measure_process(cmd)
                    sample['command']=cmd
                    record['samples'].append(sample)
                    with log.open('a') as out:out.write(output)
            if fingerprint([ROOT/p for p in record['signature']['sha256']])!=record['signature']['sha256']:
                raise RuntimeError('Benchmark inputs changed; repeat measurements')
            record['status']='passed'
        except Exception as exc:
            record['status']='failed'
            record['error']=str(exc)
            with log.open('a') as out:out.write(str(exc))
        if progress:
            progress(measured, len(compiled), '测量 ' + record['id'], record['status'],
                     '日志：' + str(ROOT / record['log']) if record['status'] == 'failed' else None)
    report={'runId':folder.name,'hardware':hardware(),'seed':config.PERF_SEED,'timing':'1 warmup + 7 samples; steady_clock; sequential processes; input generation excluded; algorithm construction included','records':[r for r,_,_ in compiled]}
    write_json(folder/'Results.json',report)
    write_json(config.REPORTS/'LatestBenchmark.json',{'report':str((folder/'Results.json').relative_to(ROOT))})
    report['path'] = str(folder / 'Results.json')
    report['passed'] = all(r['status'] == 'passed' for r, _, _ in compiled)
    return report
