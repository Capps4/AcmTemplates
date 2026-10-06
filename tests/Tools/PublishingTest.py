"""Offline checks of document generation, snippets and publication boundaries."""
import copy
import contextlib
import io
import json
from pathlib import Path
import re
import sys
import tempfile
import subprocess
import unittest
from unittest.mock import patch, Mock

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools import Catalog, Common, Fetcher, Library, Publisher, SnippetInstaller
from CardSupport import CARD, decode
from tools.Yuque import YuqueClient, YuqueError


class CatalogTest(unittest.TestCase):
    def test_all_sources(self):
        paths = Catalog.catalog()
        self.assertEqual(len({x['path'] for x in paths.values()}), len(paths))
        self.assertTrue(all(x['path'].exists() for x in paths.values()))
        self.assertEqual(len(paths), sum(len(item.get('layers', ['code']))
                                        for item in Catalog.manifest().values()))

    def test_generic_layers_prefixes_and_dependencies(self):
        modules = {
            'Base': {'path': 'src/Base', 'dependencies': []},
            'Layered': {'path': 'src/Layered', 'dependencies': ['Base'],
                        'include': 'src/Layered/Include.hpp', 'layers': ['First', 'Second']},
            'Named': {'path': 'src/Named', 'dependencies': [],
                      'prefixes': ['_T_New', '_T_Old'], 'aliases': ['Old']},
        }
        paths = Catalog.catalog(modules)
        self.assertEqual(paths['Layered/First']['include'], 'Include.hpp')
        self.assertEqual(paths['Layered/Second']['include'], 'First.hpp')
        self.assertEqual({p.name for p in Common.headers('Layered', modules)},
                         {'code.hpp', 'First.hpp', 'Second.hpp'})
        with patch.object(Fetcher, 'catalog', return_value=paths), patch.object(Fetcher, 'code_of', return_value='int x;\n'):
            data = Fetcher.snippets()
            self.assertEqual(data['Named']['prefix'], ['_T_New', '_T_Old'])
            self.assertEqual(data['Layered/First']['prefix'], '_T_First')
            entry = {'Print to console': {'prefix': '_T_Old', 'scope': 'cpp',
                     'description': 'Log output to console', 'body': ['old']}}
            self.assertTrue(Fetcher.legacy(Path('Old.code-snippets'), entry))

    def test_body_keeps_macro_system_include(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'code.hpp'
            p.write_text('#pragma once\n#include "Include.hpp"\n#include <vector>\n#define cin something\nint x;\n')
            self.assertEqual(Catalog.code_of(p), '#include <vector>\n#define cin something\nint x;\n')

    def test_markers(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'code.hpp';p.write_text('discard\n// SNIPPET BEGIN\nint x;\n// SNIPPET END\ndiscard\n')
            self.assertEqual(Catalog.code_of(p), 'int x;\n')



class LibraryTest(unittest.TestCase):
    def build(self, text, config=None, all=False):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'Library.md';p.write_text(text, encoding='utf-8')
            return Library.build(p, config or {}, require_all=all)

    def test_complete_library(self):
        result = Library.build()
        self.assertEqual(result['provenance']['templates'], len(Catalog.catalog()))
        self.assertNotIn('<!-- @code ', result['markdown'])
        self.assertEqual(len(CARD.findall(result['lake'])), sum(t.type == 'fence' for t in Library.markdown_parser().parse(result['markdown'])))
        self.assertIn('id="rNDfs"', result['lake'])
        self.assertTrue(all(decode(c)['collapsed'] for c in CARD.findall(result['lake'])
                            if decode(c)['mode'] in ('cpp', 'python')))
        self.assertEqual(result, Library.build())

    def test_all_card_ids_unique_and_math_preserved(self):
        result = Library.build()
        cards = [decode(c) for c in re.findall(r'<card\b.*?</card>', result['lake'], re.S)]
        ids = [c['id'] for c in cards]
        self.assertEqual(len(ids), len(set(ids)))
        tokens = Library.markdown_parser().parse(Library.DOCUMENT.read_text(encoding='utf-8'))
        expected = [child.content for token in tokens for child in (token.children or [])
                    if child.type == 'math']
        actual = [decode(card)['code'] for card in re.findall(
            r'<card\b[^>]*\bname="math"[^>]*>.*?</card>', result['lake'], re.S)]
        self.assertEqual(actual, expected)

    def test_source_fidelity_and_existing_card_id(self):
        key = next(iter(Catalog.catalog()))
        config = {'cards': {key: {'id': 'old', 'theme': 'custom'}}}
        result = self.build('# Template\n\n<!-- @code ' + key + ' -->\n', config)
        data = decode(CARD.findall(result['lake'])[0])
        self.assertEqual(data['code'], Catalog.code_of(Catalog.catalog()[key]['path']))
        self.assertEqual(data['id'], 'old');self.assertEqual(data['theme'], 'custom')

    def test_missing(self):
        with self.assertRaisesRegex(ValueError, '未引用'):
            self.build('# Title\n', all=True)

    def test_unknown(self):
        with self.assertRaisesRegex(ValueError, '找不到'):
            self.build('<!-- @code Missing -->\n')

    def test_duplicate(self):
        with self.assertRaisesRegex(ValueError, '重复'):
            key = next(iter(Catalog.catalog()))
            self.build('<!-- @code ' + key + ' -->\n\n<!-- @code ' + key + ' -->\n')

    def test_marker_in_example_is_literal(self):
        result = self.build('```text\n<!-- @code Missing -->\n```\n')
        data = decode(CARD.findall(result['lake'])[0])
        self.assertEqual(data['code'], '<!-- @code Missing -->\n')

    def test_markdown_features(self):
        result = self.build('# Title\n\n**加粗** [link](https://example.com)\n\n- one\n- two\n\n| a | b |\n| - | - |\n| x | y |\n\n$a+b$\n\n$$\nx^2\n$$\n')
        for tag in ['<strong>', '<a ', '<ul>', '<table>', 'name="math"']:
            self.assertIn(tag, result['lake'])
        self.assertEqual(result['lake'].count('name="math"'), 2)

    def test_html_is_not_silently_lost(self):
        for text in ['<details>hello</details>\n', 'hello <font>world</font>\n']:
            with self.assertRaisesRegex(ValueError, 'HTML'):
                self.build(text)

    def test_duplicate_heading(self):
        with self.assertRaisesRegex(ValueError, '标题重复'):
            self.build('# Same\n\n# Same\n')

    def test_tool_reference(self):
        result = self.build('<!-- @tool CTL -->\n')
        self.assertEqual(decode(CARD.findall(result['lake'])[0])['mode'], 'python')


class FetcherTest(unittest.TestCase):
    def test_all_snippet_roundtrips(self):
        data = json.loads(json.dumps(Fetcher.snippets()))
        self.assertEqual(set(data), set(Catalog.catalog()))
        for key, entry in data.items():
            body = '\n'.join(entry['body'][:-1]) + '\n'
            raw = re.sub(r'\\([\\$}])', r'\1', body)
            self.assertEqual(raw, Catalog.code_of(Catalog.catalog()[key]['path']))
            self.assertEqual(entry['body'][-1], '$0')
        self.assertNotIn('_T_', [v['prefix'] for v in data.values()])
        for key, item in Catalog.catalog().items():
            self.assertEqual(Fetcher.prefixes(data[key]), item['prefixes'])

    def test_platform_paths(self):
        with tempfile.TemporaryDirectory() as tmp:
            home = Path(tmp)
            for system, folder, env in [('Darwin', home / 'Library/Application Support/Code/User', {}),
                                        ('Windows', home / 'Roaming/Code/User', {'APPDATA': str(home / 'Roaming')}),
                                        ('Linux', home / 'xdg/Code/User', {'XDG_CONFIG_HOME': str(home / 'xdg')})]:
                folder.mkdir(parents=True)
                paths = Fetcher.candidates(home, system, env)
                self.assertIn((folder / 'snippets').resolve(), paths)

    def test_portable_server_profiles(self):
        with tempfile.TemporaryDirectory() as tmp:
            home = Path(tmp);portable = home / 'portable'
            user = portable / 'user-data/User';user.mkdir(parents=True)
            profile = user / 'profiles/123';profile.mkdir(parents=True)
            server = home / '.vscode-server/data/User';server.mkdir(parents=True)
            paths = Fetcher.candidates(home, 'Linux', {'VSCODE_PORTABLE': str(portable)})
            self.assertIn((user / 'snippets').resolve(), paths);self.assertIn((profile / 'snippets').resolve(), paths)
            self.assertIn((server / 'snippets').resolve(), paths)

    def test_portable_detected_from_executable(self):
        with tempfile.TemporaryDirectory() as tmp:
            home = Path(tmp)
            binary = home / 'VSCode/bin/code'
            binary.parent.mkdir(parents=True);binary.write_text('')
            user = home / 'VSCode/data/user-data/User';user.mkdir(parents=True)
            with patch.object(Fetcher.shutil, 'which', side_effect=lambda name: str(binary) if name == 'code' else None):
                self.assertIn((user / 'snippets').resolve(), Fetcher.candidates(home, 'Linux', {}))

    def test_ambiguous_and_explicit(self):
        with patch.object(Fetcher, 'candidates', return_value=[Path('/a'), Path('/b')]), patch.object(SnippetInstaller.sys.stdin, 'isatty', return_value=False), patch('builtins.input', side_effect=AssertionError('unexpected input')):
            with self.assertRaisesRegex(ValueError, '多个'):
                Fetcher.locate()
            self.assertEqual(Fetcher.locate('/chosen'), Path('/chosen'))
        self.assertEqual(Fetcher.locate(user_data='/tmp/data'), Path('/tmp/data/User/snippets').resolve())

    def test_no_configuration(self):
        with patch.object(Fetcher, 'candidates', return_value=[]), patch.object(SnippetInstaller.sys.stdin, 'isatty', return_value=False), patch('builtins.input', side_effect=AssertionError('unexpected input')):
            with self.assertRaisesRegex(ValueError, '未找到'):
                Fetcher.locate()

    def test_interactive_choice_and_custom_directory(self):
        with patch.object(SnippetInstaller.sys.stdin, 'isatty', return_value=True), contextlib.redirect_stdout(io.StringIO()):
            with patch('builtins.input', side_effect=['bad', '3', '2']):
                self.assertEqual(SnippetInstaller.locate(paths=[Path('/a'), Path('/b')]), Path('/b'))
            with patch('builtins.input', side_effect=['0', '/custom/snippets']):
                self.assertEqual(SnippetInstaller.locate(paths=[Path('/a'), Path('/b')]), Path('/custom/snippets'))
            with patch('builtins.input', return_value=''):
                with self.assertRaisesRegex(ValueError, '取消'):
                    SnippetInstaller.locate(paths=[])

    def test_jsonc(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp) / 'cpp.json'
            p.write_text('// comment\n{"mine":{"prefix":"abc", "body":["https://x/", ",}",],},}')
            self.assertEqual(Fetcher.load(p)['mine']['body'], ['https://x/', ',}'])

    def test_legacy_migration_preserves_personal_and_is_idempotent(self):
        with tempfile.TemporaryDirectory() as tmp, patch.object(Fetcher, 'CACHE', Path(tmp) / 'repo/.cache'):
            dest = Path(tmp) / 'snippets';dest.mkdir()
            personal = dest / 'init_.code-snippets'
            original = '{"main":{"prefix":"_T_","body":["int main() {}"]}}'
            personal.write_text(original)
            name = next(iter(Catalog.catalog().values()))['name']
            old = dest / (name + '.code-snippets')
            entry = {'Print to console': {'prefix': '_T_' + name, 'scope': 'cpp',
                     'description': 'Log output to console', 'body': ['old', '$0']}}
            old.write_text(json.dumps(entry));old_text = old.read_text()
            self.assertEqual(Fetcher.plan(dest), [old])
            with self.assertRaisesRegex(ValueError, '旧模板'):
                Fetcher.install(dest)
            result = Fetcher.install(dest, migrate=True)
            self.assertFalse(old.exists());self.assertEqual(personal.read_text(), original)
            self.assertEqual((Path(result['backup']) / old.name).read_text(), old_text)
            self.assertFalse(Fetcher.install(dest)['changed'])

    def test_unknown_conflict_and_unowned_target_refused(self):
        with tempfile.TemporaryDirectory() as tmp:
            dest = Path(tmp)
            entry = next(iter(Fetcher.snippets().values()))
            prefix = Fetcher.prefixes(entry)[0]
            (dest / 'cpp.json').write_text(json.dumps({'mine': {'prefix': prefix, 'body': []}}))
            with self.assertRaisesRegex(ValueError, '重复前缀'):
                Fetcher.plan(dest)
            (dest / 'cpp.json').unlink()
            (dest / Fetcher.FILENAME).write_text('{"mine":{"prefix":"xyz"}}')
            with self.assertRaisesRegex(ValueError, '非本工具'):
                Fetcher.plan(dest)

    def test_failed_install_rolls_back(self):
        with tempfile.TemporaryDirectory() as tmp, patch.object(Fetcher, 'CACHE', Path(tmp) / 'repo/.cache'):
            dest = Path(tmp) / 'snippets';dest.mkdir()
            name = next(iter(Catalog.catalog().values()))['name']
            old = dest / (name + '.code-snippets')
            old.write_text(json.dumps({'Print to console': {'prefix': '_T_' + name, 'scope': 'cpp',
                'description': 'Log output to console', 'body': ['old']}}))
            original_unlink = Path.unlink
            def fail(path, *args, **kwargs):
                if path == old:
                    raise OSError('simulated delete failure')
                return original_unlink(path, *args, **kwargs)
            with patch.object(Path, 'unlink', fail):
                with self.assertRaises(OSError):
                    Fetcher.install(dest, True)
            self.assertTrue(old.exists());self.assertFalse((dest / Fetcher.FILENAME).exists())


class PublishTest(unittest.TestCase):
    def console(self):
        from tools.Console import Console
        import io
        return Console('test', io.StringIO())

    def fixture(self):
        temporary = tempfile.TemporaryDirectory();self.addCleanup(temporary.cleanup)
        prepared = Path(temporary.name) / 'fixture.lake';prepared.write_text('test lake')
        report = {'ref': 'owner/book/doc', 'changed': True, 'remoteChanged': False,
                  'prepared': str(prepared), 'diff': 'diff', 'revision': 'old'}
        return report, {'id': 1, '_yuque': {'revision': 'old'}}

    def test_conflict_prevents_write(self):
        values = self.fixture();values[0]['remoteChanged'] = True
        client = Mock()
        with patch.object(Publisher, 'prepare', return_value=values), patch.object(Publisher, 'build', return_value={'provenance': {'templates': 55}}):
            with self.assertRaisesRegex(YuqueError, '远端'):
                Publisher.publish(self.console(), client)
        client.update.assert_not_called()

    def test_fresh_verification_required(self):
        client = Mock()
        with patch.object(Publisher, 'prepare', return_value=self.fixture()), patch.object(Publisher, 'build', return_value={'provenance': {'templates': 55}}), patch('tools.Verification.save', return_value=({'passed': False}, 'report')):
            with self.assertRaisesRegex(YuqueError, '验证'):
                Publisher.publish(self.console(), client)
        client.update.assert_not_called()

    def test_verified_publish_and_noop(self):
        client = Mock();client.update.return_value = {'_yuque': {'revision': 'new', 'verified': True, 'backup': 'backup'}}
        with tempfile.TemporaryDirectory() as tmp, patch.object(Publisher, 'STATE', Path(tmp)), patch.object(Publisher, 'prepare', return_value=self.fixture()) as prepared, patch('tools.Verification.save', return_value=({'passed': True}, 'report')), patch.object(Publisher, 'build', return_value={'provenance': {'templates': 55}, 'lake': 'test lake'}):
            self.assertTrue(Publisher.publish(self.console(), client)['verified'])
            self.assertEqual(json.loads(Publisher.state_path('owner/book/doc').read_text())['revision'], 'new')
            prepared.return_value[0]['changed'] = False;client.update.reset_mock()
            self.assertFalse(Publisher.publish(self.console(), client)['applied'])
            client.update.assert_not_called()

    def test_failed_readback_does_not_advance_revision(self):
        client = Mock();client.update.return_value = {'_yuque': {'revision': 'new', 'verified': False}}
        with tempfile.TemporaryDirectory() as tmp, patch.object(Publisher, 'STATE', Path(tmp)), patch.object(Publisher, 'prepare', return_value=self.fixture()), patch('tools.Verification.save', return_value=({'passed': True}, 'report')), patch.object(Publisher, 'build', return_value={'provenance': {'templates': 55}, 'lake': 'test lake'}):
            with self.assertRaisesRegex(YuqueError, '回读'):
                Publisher.publish(self.console(), client)
            self.assertFalse(Publisher.state_path('owner/book/doc').exists())

    def test_missing_token_keeps_offline_capabilities(self):
        with patch('tools.Yuque.read_token', return_value=None):
            with self.assertRaisesRegex(YuqueError, 'Token'):
                YuqueClient()
            self.assertEqual(set(Fetcher.snippets()), set(Catalog.catalog()))
            self.assertEqual(Library.build()['provenance']['templates'], len(Catalog.catalog()))

    def test_token_redacted_on_auth_failure(self):
        client = object.__new__(YuqueClient);client.token, client.node = 'fake-test-token', 'node'
        failure = subprocess.CompletedProcess([], 1, '', '403 fake-test-token denied')
        with patch('tools.Yuque.subprocess.run', return_value=failure):
            with self.assertRaises(YuqueError) as result:
                client.status()
        self.assertNotIn(client.token, str(result.exception))
        self.assertIn('[REDACTED]', str(result.exception))

    def test_timeout_is_not_retried(self):
        client = object.__new__(YuqueClient);client.token, client.node = 'fake-test-token', 'node'
        with patch('tools.Yuque.subprocess.run', side_effect=subprocess.TimeoutExpired('node', 150)) as run:
            with self.assertRaisesRegex(YuqueError, '写入可能'):
                client.status()
            self.assertEqual(run.call_count, 1)


if __name__ == '__main__':
    unittest.main()
