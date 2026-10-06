"""Exercise the internal runner in an isolated miniature library."""
import fcntl
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools.Common import ROOT, execute


class InfrastructureTest(unittest.TestCase):
    def test_actual_generators(self):
        from tools.testing.Options import source, MODULES
        from tools.testing.Integration import assemble
        self.assertEqual(len(MODULES), 6)
        self.assertIn('Template commented options enabled', source())
        self.assertIn('Integration/Test.hpp', assemble())

    def test_workflow(self):
        with tempfile.TemporaryDirectory(prefix='template-infra-') as tmp:
            root = Path(tmp)
            (root / 'tools/testing').mkdir(parents=True)
            for name in ('__init__.py', 'Run.py', 'Common.py', 'Catalog.py'):
                shutil.copy2(ROOT / 'tools' / name, root / 'tools' / name)
            (root / 'tools/testing/__init__.py').write_text('')
            for name in ('Tiny', 'Dep', 'Third'):
                (root / 'src' / name).mkdir(parents=True)
                (root / 'tests/Correctness' / name).mkdir(parents=True)
            (root / 'tests/Correctness/Integration').mkdir()
            (root / 'tests/Support').mkdir()
            (root / 'tests/Correctness/Integration/Test.hpp').write_text('// fixture\n')
            shutil.copy2(ROOT / 'tests/Support/TestSupport.hpp', root / 'tests/Support/TestSupport.hpp')
            (root / 'Manifest.json').write_text(json.dumps([
                {'module': 'Tiny', 'path': 'src/Tiny', 'dependencies': ['Dep']},
                {'module': 'Dep', 'path': 'src/Dep', 'dependencies': []},
                {'module': 'Third', 'path': 'src/Third', 'dependencies': []}]))
            (root / 'src/Third/code.hpp').write_text('#pragma once\ninline int third() { return 3; }\n')
            (root / 'tests/Correctness/Third/Test.cpp').write_text('#include "../../../src/Third/code.hpp"\nint main() { return third() != 3; }\n')
            (root / 'src/Dep/code.hpp').write_text('#pragma once\ninline int dep() { return 7; }\n')
            (root / 'src/Tiny/code.hpp').write_text('#pragma once\n#include "../Dep/code.hpp"\ninline int value() { return dep(); }\n')
            test = '#include "../../../src/Tiny/code.hpp"\n#include "../../Support/TestSupport.hpp"\nint main() { CHECK(value() == 7); }\n'
            (root / 'tests/Correctness/Tiny/Test.cpp').write_text(test)
            (root / 'tests/Correctness/Dep/Test.cpp').write_text('#include "../../../src/Dep/code.hpp"\nint main() { return dep() != 7; }\n')
            (root / 'tools/testing/Integration.py').write_text(
                'from tools.Common import ROOT, manifest\ndef inputs():\n    return manifest()\n'
                'def assemble(modules=None):\n    return \'#include "\' + str(ROOT / \'src/Tiny/code.hpp\') + \'"\\nint main() { return value() != 7; }\\n\'\n')
            (root / 'tools/testing/Options.py').write_text('MODULES = []\ndef source():\n    return "int main() {}\\n"\n')

            def command(names=None, seed=20261001):
                return [sys.executable, '-B', '-c',
                        'import tools,sys;tools.RIGHT_SEED=' + str(seed) + ';'
                        'from tools.Run import run;r=run(' + repr(names) + ');sys.exit(not r["passed"])']

            def run(names=None, seed=20261001, ok=True):
                result = subprocess.run(command(names, seed), cwd=root, text=True, capture_output=True, timeout=120)
                self.assertEqual(result.returncode == 0, ok, result.stdout + result.stderr)
                path = json.loads((root / '.cache/reports/Latest.json').read_text())['report']
                return json.loads((root / path).read_text())

            self.assertEqual(len(run(['Tiny'])['records']), 3)
            self.assertTrue(all(r['cached'] for r in run(['Tiny'])['records']))
            with (root / 'src/Dep/code.hpp').open('a') as out:
                out.write('// dependency edited\n')
            self.assertTrue(all(not r['cached'] for r in run(['Tiny'])['records']))
            with (root / 'tests/Correctness/Tiny/Test.cpp').open('a') as out:
                out.write('// test edited\n')
            self.assertTrue(all(not r['cached'] for r in run(['Tiny'])['records']))
            self.assertEqual(run(['Tiny'], seed=42)['records'][0]['signature']['seed'], 42)
            case = root / 'tests/Correctness/Tiny/TestCaseRegression.cpp';case.write_text(test)
            self.assertEqual(len(run(['Tiny'])['records']), 6)
            case.unlink()
            (root / 'tests/Correctness/Tiny/Test.cpp').write_text(test.replace('== 7', '== 8'))
            report = run(['Tiny'], ok=False)
            self.assertTrue(all('value() == 8' in r['error'] for r in report['records']))
            (root / 'tests/Correctness/Tiny/Test.cpp').write_text(test)
            header = root / 'src/Tiny/code.hpp';good = header.read_text();header.write_text('invalid C++\n')
            self.assertTrue(all(not r['cached'] for r in run(['Tiny'], ok=False)['records']))
            run(['Tiny'], ok=False)
            header.write_text(good);run(['Tiny'])
            case.write_text('#include "missing.hpp"\n')
            report = run(['Tiny'], ok=False)
            self.assertEqual(sum(r['status'] == 'failed' for r in report['records']), 3)
            case.unlink()
            case.write_text('int main() { volatile int i = 3; int* p = new int[1]; p[i] = 7; delete[] p; }\n')
            report = run(['Tiny'], ok=False)
            self.assertTrue(any('AddressSanitizer' in r.get('error', '') for r in report['records']))
            case.write_text('#include <climits>\nint main() { volatile int x = INT_MAX; int y = x + 1; return y == 0; }\n')
            report = run(['Tiny'], ok=False)
            self.assertTrue(any('runtime error' in r.get('error', '') for r in report['records']))
            case.unlink()
            self.assertEqual({r['module'] for r in run()['records']}, {'Tiny', 'Dep', 'Third', 'Integration'})
            with (root / '.cache/reports/.run.lock').open('a') as lock:
                fcntl.flock(lock, fcntl.LOCK_EX)
                process = subprocess.Popen(command(['Tiny']), cwd=root, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                try:
                    time.sleep(.2)
                    self.assertIsNone(process.poll())
                finally:
                    fcntl.flock(lock, fcntl.LOCK_UN)
                output, error = process.communicate(timeout=30)
                self.assertEqual(process.returncode, 0, output + error)
        with self.assertRaisesRegex(RuntimeError, 'Timeout'):
            execute([sys.executable, '-c', 'import time; time.sleep(1)'], timeout=.05)


if __name__ == '__main__':
    unittest.main()
