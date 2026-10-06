"""CTL command routing and human-readable output, without real updates or full tests."""
import contextlib
import io
from pathlib import Path
import sys
import unittest
from unittest.mock import Mock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools import Ctl
from tools.Console import Console


class CtlTest(unittest.TestCase):
    def invoke(self, arguments):
        output = io.StringIO()
        with contextlib.redirect_stdout(output):
            result = Ctl.main(arguments)
        return result, output.getvalue()

    def test_help_and_removed_commands(self):
        with contextlib.redirect_stdout(io.StringIO()) as output:
            with self.assertRaises(SystemExit) as result:
                Ctl.main(['--help'])
        self.assertEqual(result.exception.code, 0)
        self.assertIn('ctl update --local', output.getvalue())
        for command in [['export'], ['doctor'], ['doc'], ['test'], ['update'],
                        ['test', '--right', '--perf'], ['update', '--local', '--yuque']]:
            with contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(SystemExit) as result:
                    Ctl.main(command)
            self.assertEqual(result.exception.code, 2)

    def test_local_update_routing(self):
        with patch.object(Ctl, 'update_local', return_value=0) as local:
            self.assertEqual(self.invoke(['update', '--local'])[0], 0)
            local.assert_called_once()

    def test_yuque_update_routing(self):
        with patch('tools.Publisher.publish', return_value={'applied': False}) as publish:
            status, output = self.invoke(['update', '--yuque'])
        self.assertEqual(status, 0);publish.assert_called_once()
        self.assertIn('无需写入', output)

    def test_rule_routing(self):
        with patch.object(Ctl, 'rule', return_value=0) as rule:
            self.assertEqual(self.invoke(['rule'])[0], 0)
            rule.assert_called_once()

    def test_test_modes(self):
        for mode, flags in [('--right', (True, False)), ('--perf', (False, True)), ('--all', (True, True))]:
            with patch.object(Ctl, 'test', return_value=0) as test:
                self.assertEqual(self.invoke(['test', mode])[0], 0)
                self.assertEqual(test.call_args.args[1:], flags)

    def test_both_categories_run_after_correctness_failure(self):
        right = {'records': [{'status': 'failed'}], 'passed': False, 'path': 'right-report'}
        perf = {'records': [{'status': 'passed'}], 'passed': True, 'path': 'perf-report'}
        with patch('tools.Ctl.platform.system', return_value='Darwin'), patch('tools.Run.run', return_value=right) as run, patch('tools.Benchmark.run', return_value=perf) as bench, patch('tools.Verification.save', return_value=({'passed': False}, 'summary')):
            status, output = self.invoke(['test', '--all'])
        self.assertEqual(status, 1);run.assert_called_once();bench.assert_called_once()
        self.assertIn('性能测试', output);self.assertIn('失败', output)

    def test_errors_have_no_json_or_traceback(self):
        with patch.object(Ctl, 'update_local', side_effect=ValueError('multiple directories')):
            status, output = self.invoke(['update', '--local'])
        self.assertNotEqual(status, 0)
        self.assertIn('multiple directories', output);self.assertNotIn('Traceback', output)
        self.assertNotIn('"ok"', output)

    def test_non_tty_output_has_no_ansi(self):
        output = io.StringIO();console = Console('Test', output)
        with console.step('Stage'):
            console.progress(1, 2, 'Module/strict', 'cached')
        console.finish('Done')
        text = output.getvalue()
        self.assertNotIn('\033', text);self.assertIn('[1/2]', text)
        self.assertIn('复用缓存', text);self.assertIn('完成', text)

    def test_local_pipeline_uses_init_configuration(self):
        output = io.StringIO()
        with patch('tools.Fetcher.snippets', return_value={'x': {}}), patch('tools.Fetcher.locate', return_value=Path('/tmp/snippets')) as locate, patch('tools.Fetcher.plan', return_value=[]), patch('tools.Fetcher.install', return_value={'installed': '/tmp/snippets/ctl.code-snippets', 'changed': False}) as install, patch('tools.Ctl.config.SNIPPETS_DIR', '/tmp/snippets'):
            self.assertEqual(Ctl.update_local(Console('Test', output)), 0)
        self.assertEqual(locate.call_args.args[0], '/tmp/snippets')
        install.assert_called_once()
        self.assertIn('无需写入', output.getvalue())


if __name__ == '__main__':
    unittest.main()
