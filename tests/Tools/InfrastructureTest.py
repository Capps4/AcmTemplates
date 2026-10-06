"""Small, separate regressions for discovery, attribution, evidence and cleanup."""
import contextlib
import copy
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import threading
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
import tools as config
from tools import Benchmark, Catalog, Common, Run, Verification


class InfrastructureTest(unittest.TestCase):
    def setUp(self):
        self.stack = contextlib.ExitStack()
        self.addCleanup(self.stack.close)
        self.root = Path(self.stack.enter_context(tempfile.TemporaryDirectory(prefix='ctl-infra-'))).resolve()
        self.folder = self.root / 'tests/Correctness/Tiny'
        self.folder.mkdir(parents=True)
        (self.root / 'src/Tiny').mkdir(parents=True)
        (self.root / 'tests/Performance/Tiny').mkdir(parents=True)
        (self.root / 'tools').mkdir()
        for name in ('__init__.py', 'Run.py', 'Benchmark.py', 'Common.py', 'Catalog.py', 'Verification.py'):
            shutil.copy2(Common.ROOT / 'tools' / name, self.root / 'tools' / name)
        (self.root / 'src/Tiny/code.hpp').write_text('#pragma once\ninline int value() { return 7; }\n')
        (self.folder / 'Test.cpp').write_text('#include "../../../src/Tiny/code.hpp"\nint main() { return value() != 7; }\n')
        (self.root / 'Manifest.json').write_text(json.dumps([dict(module='Tiny', path='src/Tiny', dependencies=[])]))
        case = dict(id='Tiny/value', module='Tiny', header='src/Tiny/code.hpp', source='tests/Correctness/Tiny/Test.cpp', scenario='value', oracle='7')
        (self.root / 'tests/Correctness/Cases.json').write_text(json.dumps({'addedCases': [case]}))
        (self.root / 'tests/Performance/Tiny/Benchmark.cpp').write_text('int main() { return 0; }\n')
        for module in (Common, Catalog, Run, Benchmark, Verification):
            self.stack.enter_context(patch.object(module, 'ROOT', self.root))
        self.stack.enter_context(patch.object(Common, 'SOURCE', self.root / 'src'))
        self.stack.enter_context(patch.object(Catalog, 'SOURCE', self.root / 'src'))
        self.stack.enter_context(patch.object(Catalog, 'MANIFEST', self.root / 'Manifest.json'))
        self.stack.enter_context(patch.object(config, 'SOURCE', self.root / 'src'))
        self.stack.enter_context(patch.object(config, 'CACHE', self.root / '.cache'))
        self.stack.enter_context(patch.object(config, 'REPORTS', self.root / '.cache/reports'))
        self.stack.enter_context(patch.object(config, 'TEST_CASE', None))
        self.stack.enter_context(patch.object(Verification, 'hardware', return_value={'machine': 'fixture'}))
        self.stack.enter_context(patch.object(Benchmark, 'hardware', return_value={'machine': 'fixture'}))
        self.comp = {'path': '/fixture/compiler', 'version': 'fixture', 'target': 'fixture'}
        self.stack.enter_context(patch.object(Run, 'compiler', return_value=self.comp))
        self.stack.enter_context(patch.object(Benchmark, 'compiler', return_value=self.comp))

    def test_missing_include_is_attributed_to_its_entry_only(self):
        (self.folder / 'TestCaseBroken.cpp').write_text('#include "missing.hpp"\n')
        for profile in Verification.PROFILES:
            jobs = Run.module_jobs('Tiny', profile)
            self.assertEqual(len(jobs), 2)
            normal, broken = jobs
            self.assertTrue(Run.signature(normal, self.comp, config.RIGHT_SEED)['sha256'])
            with self.assertRaisesRegex(RuntimeError, 'missing.hpp'):
                Run.signature(broken, self.comp, config.RIGHT_SEED)

    def test_include_symlink_retargeting_invalidates_source_signature(self):
        first, second = self.folder / 'First.hpp', self.folder / 'Second.hpp'
        first.write_text('constexpr int selected = 1;\n')
        second.write_text('constexpr int selected = 2;\n')
        link = self.folder / 'Selected.hpp'; link.symlink_to(first)
        (self.folder / 'Test.cpp').write_text('#include "Selected.hpp"\nint main() { return selected; }\n')
        job = Run.module_jobs('Tiny', 'optimized')[0]
        before = Run.signature(job, self.comp, config.RIGHT_SEED)
        link.unlink(); link.symlink_to(second)
        self.assertNotEqual(before, Run.signature(job, self.comp, config.RIGHT_SEED))

    def test_inventory_and_active_dependencies_invalidate_evidence(self):
        job = Run.module_jobs('Tiny', 'optimized')[0]
        before = Run.signature(job, self.comp, config.RIGHT_SEED)
        data = self.folder / 'Input.bin'; data.write_bytes(b'\xff\x00')
        changed = Run.signature(Run.module_jobs('Tiny', 'optimized')[0], self.comp, config.RIGHT_SEED)
        self.assertNotEqual(before, changed)
        data.write_bytes(b'\xff\x01')
        self.assertNotEqual(changed, Run.signature(Run.module_jobs('Tiny', 'optimized')[0], self.comp, config.RIGHT_SEED))
        with (self.root / 'src/Tiny/code.hpp').open('a') as out: out.write('// changed\n')
        self.assertNotEqual(before, Run.signature(job, self.comp, config.RIGHT_SEED))
        self.assertNotEqual(before, Run.signature(job, self.comp, 42))

    def test_multi_translation_unit_and_isolated_discovery(self):
        (self.folder / 'TestExtra.cpp').write_text('int witness() { return 7; }\n')
        (self.folder / 'Demo.cpp').write_text('int main() {}\n')
        (self.folder / 'BoundaryCases.hpp').write_text('// included suite\n')
        jobs = Run.module_jobs('Tiny', 'strict')
        self.assertEqual(len(jobs), 1)
        self.assertEqual([p.name for p in jobs[0]['sources']], ['Test.cpp', 'TestExtra.cpp'])
        (self.folder / 'TestCaseConfig.cpp').write_text('int main() {}\n')
        self.assertEqual(len(Run.module_jobs('Tiny', 'strict')), 2)

    def test_failure_keeps_partial_case_output_and_termination(self):
        for code, timeout, reason in [(1, None, 'Exit 1'), (-11, None, 'Signal'), (None, .01, 'Timeout')]:
            error = Common.ProcessError(['program'], 'CASE_BEGIN Tiny/broken\n', 'expected=7; actual=8\n', code, timeout)
            record = Run.failure({'id': 'Tiny/optimized/main', 'phase': 'run'}, error)
            self.assertEqual(record['failedCase'], 'Tiny/broken')
            self.assertEqual(record['returncode'], code)
            self.assertIn(reason, record['error'])
            self.assertIn('expected=7; actual=8', record['error'])

    def right_records(self):
        return [dict(id=job['id'], module='Tiny', profile=profile, status='passed', caseIds=['Tiny/value'],
                     runMs=1, caseMs={'Tiny/value': 1}, signature=Run.signature(job, self.comp, config.RIGHT_SEED))
                for profile in Verification.PROFILES for job in Run.module_jobs('Tiny', profile)]

    def perf_record(self):
        source = self.root / 'tests/Performance/Tiny/Benchmark.cpp'
        return dict(id='Tiny/Benchmark', module='Tiny', source=str(source.relative_to(self.root)), status='passed',
                    signature=Benchmark.signature('Tiny', source), samples=[dict(n=n, shape=shape, seed=config.PERF_SEED,
                    iterations=1, warmupMs=.1, samplesMs=[.1] * config.PERF_REPEATS) for n in config.PERF_DEFAULT_SIZES for shape in (0, 1)])

    def test_report_round_trip_cleanup_and_preserve_other_workflows(self):
        for name in ('build', 'benchmarks', 'generated', 'reports/20261006T010000Z-aabbccdd',
                     'reports/20261006T010000Z-benchmark-aabbccdd', 'reports/20261006T010000Z-special-aabbccdd',
                     'reports/benchmark-merged-20261006T010000Z'):
            path = config.CACHE / name; path.mkdir(parents=True); (path / 'keep-or-remove').write_text('x')
        (config.REPORTS / 'Rules.json').write_text('{}')
        (config.REPORTS / 'Latest.json').write_text('{}')
        data, path = Verification.save(right=self.right_records(), perf=[self.perf_record()])
        self.assertTrue(data['passed'])
        self.assertEqual(path.name, 'Verification.md')
        self.assertIn('| Tiny | ✓ |', path.read_text())
        self.assertEqual(len(Verification.read_evidence()['right']), 3)
        self.assertFalse((config.CACHE / 'build').exists())
        self.assertFalse((config.CACHE / 'benchmarks').exists())
        self.assertFalse(list(config.REPORTS.glob('20261006T010000Z*')))
        self.assertFalse((config.REPORTS / 'benchmark-merged-20261006T010000Z').exists())
        self.assertFalse((config.REPORTS / 'Latest.json').exists())
        self.assertTrue((config.REPORTS / 'Rules.json').exists())
        self.assertTrue((config.CACHE / 'generated').exists())
        self.assertFalse((config.REPORTS / '20261006T010000Z-aabbccdd').exists())

    def test_partial_profile_case_and_interruption_cannot_publish(self):
        records = self.right_records()
        Verification.save(right=records, perf=[self.perf_record()])
        data, _ = Verification.save(right=records[:1])
        self.assertFalse(data['passed'])
        self.assertEqual(len(Verification.read_evidence()['right']), 1)
        filtered = copy.deepcopy(records)
        for record in filtered: record['signature']['case'] = 'value'
        self.assertFalse(Verification.save(right=filtered)[0]['passed'])
        records[0].update(status='interrupted', phase='pending')
        self.assertFalse(Verification.save(right=records)[0]['passed'])

    def test_case_catalog_is_coverage_not_a_count_quota(self):
        evidence = dict(right=self.right_records(), perf=[self.perf_record()])
        self.assertTrue(Verification.collect(evidence)['passed'])  # A single real risk suffices.
        for record in evidence['right']: record['caseIds'] = []
        self.assertFalse(Verification.collect(evidence)['passed'])

    def test_commands_close_stdin_and_preserve_explicit_input(self):
        script = 'import sys; print(repr(sys.stdin.read()))'
        # Give the parent readable input; the nested batch command must see EOF.
        child = ('from tools.Common import execute; import sys; '
                 'print(execute([sys.executable, "-c", ' + repr(script) + '], timeout=5).stdout, end="")')
        with tempfile.TemporaryFile(mode='w+') as inherited:
            inherited.write('unintended input'); inherited.seek(0)
            process = subprocess.run([sys.executable, '-B', '-c', child], cwd=self.root,
                                     stdin=inherited, capture_output=True, text=True, timeout=10)
        self.assertEqual(process.returncode, 0, process.stderr)
        self.assertEqual(process.stdout.strip(), "''")
        result = Common.execute([sys.executable, '-c', script], input_text='expected', timeout=5)
        self.assertEqual(result.stdout.strip(), "'expected'")

    def test_explicit_performance_placeholder_and_stale_evidence(self):
        folder = self.root / 'tests/Performance/Tiny'
        settings = folder / 'Settings.json'
        settings.write_text(json.dumps({'skip': 'output utility'}))
        (folder / 'Benchmark.cpp').write_text('// intentionally empty performance placeholder\n')
        with patch.object(Benchmark, 'compile_one') as compile_one, patch.object(Benchmark, 'measure_process') as measure:
            result = Benchmark.run(['Tiny'])
            compile_one.assert_not_called(); measure.assert_not_called()
        self.assertTrue(result['passed'])
        record = result['records'][0]
        self.assertEqual(record['status'], 'skipped')
        self.assertEqual(record['samples'], [])
        data, path = Verification.save(right=self.right_records())
        self.assertTrue(data['passed'])
        self.assertFalse(data['failures'])
        self.assertIn('无需性能测试', path.read_text())
        with (self.root / 'src/Tiny/code.hpp').open('a') as out:
            out.write('// changed\n')
        self.assertFalse(Benchmark.current(record))
        fresh = Benchmark.skipped_record('Tiny', folder / 'Benchmark.cpp')
        self.assertTrue(Benchmark.current(fresh))
        settings.unlink()
        self.assertFalse(Benchmark.current(fresh))
        (folder / 'Benchmark.cpp').unlink()
        self.assertFalse(Benchmark.run(['Tiny'])['passed'])

    def test_performance_skip_requires_reason_and_existing_entry(self):
        folder = self.root / 'tests/Performance/Tiny'
        path = folder / 'Settings.json'
        for reason in ('', '  ', True, None, []):
            path.write_text(json.dumps({'skip': reason}))
            with self.assertRaisesRegex(ValueError, '非空原因'):
                Benchmark.run(['Tiny'])
        path.write_text(json.dumps({'skip': 'output utility'}))
        (folder / 'Benchmark.cpp').unlink()
        self.assertFalse(Benchmark.run(['Tiny'])['passed'])

    def test_benchmark_settings_and_malformed_measurements(self):
        path = self.root / 'tests/Performance/Tiny/Settings.json'
        self.assertEqual(Benchmark.workload_sizes('Tiny'), config.PERF_DEFAULT_SIZES)
        for sizes in ([0, 2], [2, 2], [3, 2], [1], [True, 2], ['1', 2]):
            path.write_text(json.dumps({'sizes': sizes}))
            with self.assertRaises(ValueError): Benchmark.workload_sizes('Tiny')
        for iterations in (0, -1, True, 1.5, '2', {'Benchmark': 0}):
            path.write_text(json.dumps({'iterations': iterations}))
            with self.assertRaises(ValueError):
                Benchmark.workload_iterations('Tiny', path.parent / 'Benchmark.cpp')
        path.write_text(json.dumps({'iterations': {'Benchmark': 3, 'BenchmarkOther': 7}}))
        self.assertEqual(Benchmark.workload_iterations('Tiny', path.parent / 'Benchmark.cpp'), 3)
        self.assertEqual(Benchmark.workload_iterations('Tiny', path.parent / 'BenchmarkOther.cpp'), 7)
        path.unlink()
        good = self.perf_record(); self.assertTrue(Benchmark.current(good))
        for field, value in [('samplesMs', [1]), ('samplesMs', [float('nan')] * 3), ('warmupMs', -1), ('warmupMs', True)]:
            bad = copy.deepcopy(good); bad['samples'][0][field] = value
            self.assertFalse(Benchmark.current(bad))
        bad = copy.deepcopy(good); bad['samples'][1] = bad['samples'][0]
        self.assertFalse(Benchmark.current(bad))

    def test_benchmark_compilers_finish_before_serial_measurements(self):
        (self.root / 'tests/Performance/Tiny/BenchmarkExtra.cpp').write_text('int main() { return 0; }\n')
        barrier = threading.Barrier(2)
        active = 0
        guard = threading.Lock()
        def compile_fixture(name, source, folder):
            nonlocal active
            with guard: active += 1
            try:
                barrier.wait(timeout=5)
                record = dict(id=name + '/' + source.stem, module=name, source=str(source.relative_to(self.root)),
                              status='compiled', samples=[], signature=Benchmark.signature(name, source))
                return record, folder / source.stem
            finally:
                with guard: active -= 1
        calls = []
        ready = set()
        def prepare_fixture(command):
            with guard:
                self.assertEqual(active, 0)
                ready.add(command[0])
            return None
        def measure_fixture(command, prepared=None):
            with guard:
                self.assertEqual(active, 0)
                self.assertEqual(len(ready), 2)
            calls.append(command)
            return {'samples': [dict(n=int(n), shape=shape, seed=int(command[3]),
                        iterations=1, warmupMs=.1, samplesMs=[.1] * config.PERF_REPEATS)
                        for n in command[1:3] for shape in (0, 1)], 'peakRssBytes': 1234}
        with patch.object(Benchmark, 'compile_one', side_effect=compile_fixture), patch.object(Benchmark, 'prepare_process', side_effect=prepare_fixture), patch.object(Benchmark, 'measure_process', side_effect=measure_fixture), patch.object(config, 'TEST_JOBS', 2):
            result = Benchmark.run(['Tiny'])
        self.assertTrue(result['passed'])
        self.assertEqual(len(calls), 2)
        self.assertEqual(len(result['records']), 2)
        self.assertFalse(any(p.is_dir() and p.name.startswith('test-') for p in config.CACHE.iterdir()))

    def test_benchmark_process_waits_before_work_and_is_reaped(self):
        script = ('import json,sys; print("CTL_BENCHMARK_READY", file=sys.stderr, flush=True); '
                  'sys.stdin.readline(); print(json.dumps({"n": 1}), flush=True)')
        command = [sys.executable, '-c', script]
        prepared = Benchmark.prepare_process(command)
        try:
            self.assertIsNone(prepared['process'].poll())
            self.assertEqual(prepared['out'].tell(), 0)
            result = Benchmark.measure_process(command, prepared=prepared)
            self.assertEqual(result['samples'], [{'n': 1}])
            self.assertEqual(prepared['process'].returncode, 0)
            with self.assertRaises(ChildProcessError):
                import os
                os.waitpid(prepared['process'].pid, os.WNOHANG)
        finally:
            Benchmark.stop_process(prepared)

    def test_build_cache_rejects_changed_inputs_and_corrupted_binary(self):
        target = self.root / 'program'
        target.write_text('#!/bin/sh\nexit 0\n'); target.chmod(0o755)
        sig = {'compiler': self.comp, 'flags': ['strict']}
        header = self.root / 'src/Tiny/code.hpp'
        inputs = {'src/Tiny/code.hpp': Common.digest(header)}
        Common.cache_program('Tiny/main', sig, target, inputs)
        self.assertIsNotNone(Common.cached_program('Tiny/main', sig))
        self.assertIsNone(Common.cached_program('Tiny/main', {**sig, 'flags': ['optimized']}))
        binary = Common.cached_program('Tiny/main', sig)[0]
        binary.write_text('#!/bin/sh\nexit 1\n')
        self.assertIsNone(Common.cached_program('Tiny/main', sig))
        Common.cache_program('Tiny/main', sig, target, inputs)
        header.write_text('inline int value() { return 8; }\n')
        self.assertIsNone(Common.cached_program('Tiny/main', sig))
        Common.cache_program('Tiny/main', sig, target, {'src/Tiny/code.hpp': Common.digest(header)})
        binary = Common.cached_program('Tiny/main', sig)[0]
        binary.unlink()
        self.assertIsNone(Common.cached_program('Tiny/main', sig))

    def test_tiny_compiler_link_and_dependency_integration(self):
        # Only six jobs, unlike the old monolith's repeated full miniature builds.
        (self.folder / 'TestExtra.cpp').write_text('int witness() { return 7; }\n')
        (self.folder / 'Test.cpp').write_text('#include "../../../src/Tiny/code.hpp"\n#include <iostream>\n#include <fstream>\nint witness();\nint main() { std::ofstream("runs.txt", std::ios::app) << "run\\n"; std::cout << "CASE_PASS Tiny/value\\nCASE_TIME Tiny/value 0.1\\n"; return value() != witness(); }\n')
        (self.folder / 'TestCaseBroken.cpp').write_text('#include "missing.hpp"\n')
        script = ('from tools.Run import run; from tools.Verification import read_evidence; import json; '
                  'r=run(["Tiny"]); cold=read_evidence(); r=run(["Tiny"]); '
                  'print(json.dumps({"cold": cold, "warm": read_evidence()}))')
        process = subprocess.run([sys.executable, '-B', '-c', script], cwd=self.root, capture_output=True, text=True, timeout=90)
        self.assertEqual(process.returncode, 0, process.stdout + process.stderr)
        evidence = json.loads(process.stdout)
        records = evidence['warm']['right']
        self.assertEqual((self.root / 'runs.txt').read_text().splitlines(), ['run'] * 6)
        self.assertTrue(all(not r['compileCached'] for r in evidence['cold']['right'] if r['status'] == 'passed'))
        self.assertTrue(all(r['compileCached'] for r in records if r['status'] == 'passed'))
        self.assertEqual(sum(r['status'] == 'passed' for r in records), 3)
        self.assertEqual(sum(r['status'] == 'failed' for r in records), 3)
        self.assertTrue(all(r['phase'] == 'dependencies' for r in records if r['status'] == 'failed'))
        self.assertFalse(any(p.is_dir() and p.name.startswith('test-') for p in config.CACHE.iterdir()))


if __name__ == '__main__':
    unittest.main()
