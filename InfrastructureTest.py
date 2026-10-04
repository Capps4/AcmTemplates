"""Exercise the runner in an isolated miniature library, leaving templates unchanged."""
import fcntl
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time
import unittest
from Common import ROOT, execute


class InfrastructureTest(unittest.TestCase):
    def test_actual_generators(self):
        from Options import source, MODULES
        from Integration import assemble
        self.assertEqual(len(MODULES), 9)
        self.assertIn('Original commented options enabled', source())
        self.assertIn('Integration/Test.hpp', assemble())

    def test_workflow(self):
        with tempfile.TemporaryDirectory(prefix='template-infra-') as tmp:
            root = Path(tmp)
            for name in ('Run.py', 'Common.py', 'BenchmarkSupport.hpp', 'TestSupport.hpp'):
                shutil.copy2(ROOT / name, root / name)
            for name in ('Tiny', 'Dep', 'Deferred', 'Integration'):
                (root / name).mkdir()
            (root / 'Integration/Test.hpp').write_text('// integration fixture\n')
            data = [{'module': 'Tiny', 'dependencies': ['Dep']},
                    {'module': 'Dep', 'dependencies': []},
                    {'module': 'Deferred', 'dependencies': [], 'deferred': 'test exclusion'}]
            (root / 'Manifest.json').write_text(json.dumps(data))
            (root / 'Dep/Final.hpp').write_text('#pragma once\ninline int dep() { return 7; }\n')
            (root / 'Tiny/Final.hpp').write_text('#pragma once\n#include "../Dep/Final.hpp"\ninline int value() { return dep(); }\n')
            test = '#include "Final.hpp"\n#include "../TestSupport.hpp"\nint main() { CHECK(value() == 7); }\n'
            (root / 'Tiny/Test.cpp').write_text(test)
            (root / 'Dep/Test.cpp').write_text('#include "Final.hpp"\nint main() { return dep() != 7; }\n')
            benchmark = '#include "Final.hpp"\n#include "../BenchmarkSupport.hpp"\nint main() { compare("tiny", [] { return 7; }, [] { return value(); }); }\n'
            (root / 'Tiny/Benchmark.cpp').write_text(benchmark)
            (root / 'Integration.py').write_text('''from Common import ROOT, manifest

def inputs():
    return {n: v for n, v in manifest().items() if not v.get('deferred')}
def assemble(modules=None):
    return '#include "' + str(ROOT / 'Tiny/Final.hpp') + '"\\nint main() { return value() != 7; }\\n'
''')
            (root / 'Options.py').write_text('MODULES = []\ndef source():\n    return "int main() {}\\n"\n')

            def run(*args, ok=True):
                result = subprocess.run([sys.executable, str(root / 'Run.py'), *args],
                                        text=True, capture_output=True, timeout=120)
                self.assertEqual(result.returncode == 0, ok, result.stdout + result.stderr)
                return result.stdout

            def latest():
                path = json.loads((root / 'Reports/Latest.json').read_text())['report']
                return json.loads((root / path).read_text())

            run('check', 'Tiny')
            self.assertEqual(len(latest()['records']), 3)
            run('check', 'Tiny')
            self.assertTrue(all(r['cached'] for r in latest()['records']))
            # Transitive include changes invalidate the affected module.
            with (root / 'Dep/Final.hpp').open('a') as out:
                out.write('// dependency edited\n')
            self.assertIn('stale', run('status'))
            run('check', 'Tiny')
            self.assertTrue(all(not r['cached'] for r in latest()['records']))
            # Test-only changes do not invalidate a performance record.
            run('bench', 'Tiny', '--min-ms', '1')
            first_bench = latest()['records'][0]
            self.assertEqual(len(first_bench['samples'][0]['final']), 7)
            self.assertEqual(first_bench['samples'][0]['checksum'], 7)
            with (root / 'Tiny/Test.cpp').open('a') as out:
                out.write('// test edited\n')
            self.assertIn('Tiny/benchmark/main: passed', run('status'))
            run('check', 'Tiny')
            self.assertTrue(all(not r['cached'] for r in latest()['records']))
            run('test', 'Tiny', '--seed', '42')
            self.assertEqual(latest()['records'][0]['signature']['seed'], 42)
            self.assertFalse(latest()['records'][0]['cached'])
            # Newly added independent cases are discovered.
            case = root / 'Tiny/TestCaseRegression.cpp'
            case.write_text(test)
            run('test', 'Tiny')
            self.assertEqual(len(latest()['records']), 2)
            case.unlink()
            # A failed CHECK carries seed and expression in the durable log.
            (root / 'Tiny/Test.cpp').write_text(test.replace('== 7', '== 8'))
            run('test', 'Tiny', ok=False)
            error = latest()['records'][0]['error']
            self.assertIn('TEST_SEED=20261001', error)
            self.assertIn('value() == 8', error)
            (root / 'Tiny/Test.cpp').write_text(test)
            # Compile failures must never be reused as successful cache entries.
            header = root / 'Tiny/Final.hpp'
            good = header.read_text()
            header.write_text('invalid C++\n')
            run('compile', 'Tiny', ok=False)
            run('compile', 'Tiny', ok=False)
            header.write_text(good)
            run('check', 'Tiny')
            # Missing includes still produce a report and retain other job results.
            case.write_text('#include "missing.hpp"\n')
            run('test', 'Tiny', ok=False)
            self.assertEqual(len(latest()['records']), 2)
            self.assertEqual(sum(r['status'] == 'failed' for r in latest()['records']), 1)
            case.unlink()
            # ASan must fail the job rather than merely emit a diagnostic.
            case.write_text('int main() { volatile int i = 3; int* p = new int[1]; p[i] = 7; delete[] p; }\n')
            run('sanitize', 'Tiny', ok=False)
            failed = [r for r in latest()['records'] if r['status'] == 'failed']
            self.assertTrue(any('AddressSanitizer' in r['error'] for r in failed))
            case.write_text('#include <climits>\nint main() { volatile int x = INT_MAX; int y = x + 1; return y == 0; }\n')
            run('sanitize', 'Tiny', ok=False)
            self.assertTrue(any('runtime error' in r.get('error', '') for r in latest()['records']))
            case.unlink()
            # Invalid checksum and stateful callbacks reject the benchmark.
            path = root / 'Tiny/Benchmark.cpp'
            path.write_text(benchmark.replace('return 7;', 'return 8;'))
            run('bench', 'Tiny', ok=False)
            self.assertIn('checksum mismatch', latest()['records'][0]['error'])
            path.write_text('#include "../BenchmarkSupport.hpp"\nint main() { int a = 0, b = 0; compare("state", [&] { return a++; }, [&] { return b++; }); }\n')
            run('bench', 'Tiny', ok=False)
            self.assertIn('changed its checksum', latest()['records'][0]['error'])
            path.write_text('#include "../BenchmarkSupport.hpp"\nint main() { compare("fraction", [] { return 1.5; }, [] { return 1.6; }); }\n')
            run('bench', 'Tiny', ok=False)
            self.assertIn('checksum mismatch', latest()['records'][0]['error'])
            # Fixed-count cold-cache fixtures can opt out of adaptive batching.
            path.write_text('#include "../BenchmarkSupport.hpp"\nint main() { int a = 0, b = 0; compare("cold", [&] { if (a++ == 9) std::exit(2); return 7; }, [&] { if (b++ == 9) std::exit(2); return 7; }, false); }\n')
            run('bench', 'Tiny')
            self.assertEqual(latest()['records'][0]['samples'][0]['batch'], 1)
            path.write_text(benchmark)
            # A baseline is immutable and its Final is actually rerun.
            run('bench', 'Tiny', '--min-ms', '1')
            run('baseline', 'accepted')
            run('baseline', 'accepted', ok=False)
            run('bench', 'Tiny', '--min-ms', '1', '--baseline', 'accepted')
            self.assertEqual(len(latest()['records'][0]['baselineComparison']['rounds']), 7)
            run('bench', 'Tiny', '--min-ms', '2', '--baseline', 'accepted', ok=False)
            # Packaging requires all current verification modes, including integration.
            run('package', ok=False)
            run('check', '--all')
            self.assertTrue(all(r['module'] != 'Deferred' for r in latest()['records']))
            run('package')
            self.assertEqual((root / 'ReadyHeaders/Tiny/Final.hpp').read_text(), good)
            self.assertFalse(list((root / 'ReadyHeaders').rglob('*.code-snippets')))
            # Runner processes serialize against the common lock.
            with (root / 'Reports/.run.lock').open('a') as lock:
                fcntl.flock(lock, fcntl.LOCK_EX)
                process = subprocess.Popen([sys.executable, str(root / 'Run.py'), 'status'],
                                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                try:
                    time.sleep(.2)
                    self.assertIsNone(process.poll())
                finally:
                    fcntl.flock(lock, fcntl.LOCK_UN)
                output, error = process.communicate(timeout=10)
                self.assertEqual(process.returncode, 0, output + error)
            run('clean')
            self.assertFalse((root / 'Build').exists())
            self.assertTrue((root / 'Baselines/accepted/Results.json').exists())
        with self.assertRaisesRegex(RuntimeError, 'Timeout'):
            execute([sys.executable, '-c', 'import time; time.sleep(1)'], timeout=.05)


if __name__ == '__main__':
    unittest.main()
