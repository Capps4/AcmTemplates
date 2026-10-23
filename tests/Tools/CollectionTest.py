"""Collection boundaries, copied installers and independent document targets."""
import base64
import contextlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
import zlib
from unittest.mock import Mock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools import Catalog, Common, Ctl, Fetcher, Library, Publisher
import tools as config


class CollectionTest(unittest.TestCase):
    def fixture(self, items):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        path = Path(tmp.name) / 'Manifest.json'
        path.write_text(json.dumps(items))
        return patch.object(Catalog, 'MANIFEST', path)

    def test_dependency_closure_and_core_boundary(self):
        items = [dict(module='Base', path='src/core/Math/Base', collection='core', dependencies=[]),
                 dict(module='Ext', path='src/extra/Math/Ext', collection='extra', dependencies=['Base'])]
        with self.fixture(items):
            self.assertEqual(set(Catalog.manifest('extra')), {'Ext'})
            self.assertEqual(set(Catalog.manifest('extra', dependencies=True)), {'Ext', 'Base'})
            self.assertEqual(set(Catalog.manifest('core', dependencies=True)), {'Base'})
        items[0]['dependencies'] = ['Ext']
        items[1]['dependencies'] = []
        with self.fixture(items), self.assertRaisesRegex(ValueError, 'core cannot'):
            Catalog.manifest()
        items[1]['dependencies'] = ['Base']
        with self.fixture(items), self.assertRaisesRegex(ValueError, 'cycle'):
            Catalog.manifest()

    def test_unknown_dependency_and_mismatched_path(self):
        items = [dict(module='X', path='src/core/Math/X', collection='core', dependencies=['Missing'])]
        with self.fixture(items), self.assertRaisesRegex(ValueError, 'Unknown dependency'):
            Catalog.manifest()
        items[0].update(path='src/extra/Math/X', dependencies=[])
        with self.fixture(items), self.assertRaisesRegex(ValueError, 'mismatch'):
            Catalog.manifest()

    def test_repository_partition_and_single_migration(self):
        core, extra = Catalog.manifest('core'), Catalog.manifest('extra')
        self.assertEqual(len(core), 42)
        self.assertEqual(set(core) & set(extra), set())
        self.assertTrue({'SparseSegTree', 'TreeMap', 'RingTree', 'MillerRabin', 'PollardRho'} <= set(core))
        migrated = set('FastFourierTransform GaussianElimination RandomNumber CountingSortOrder MoSort MinimalString DeletableHeap CentroidDecomposition Geo3 ExSuffixAutomaton PalindromeAutomaton'.split())
        self.assertTrue(migrated <= set(extra))
        self.assertTrue(all(extra[n]['legacySnippets'] for n in migrated))

    def test_repository_quoted_includes_survive_directory_migration(self):
        folders = ('src', 'Headers', 'tests/Correctness', 'tests/Performance', 'tests/Support')
        extensions = {'.c', '.cc', '.cpp', '.cxx', '.h', '.hpp', '.hxx'}
        sources = [p for folder in folders for p in (config.ROOT / folder).rglob('*')
                   if p.is_file() and p.suffix.lower() in extensions]
        self.assertTrue(sources)
        for path in Common.dependencies(sources):
            with self.subTest(path=path):
                self.assertTrue(path.is_relative_to(config.ROOT), 'quoted include escapes repository')

    def test_installer_contains_core_only_and_removes_migrated_legacy_names(self):
        snippets = Fetcher.snippets()
        self.assertEqual(set(snippets), set(Catalog.catalog(collection='core')))
        self.assertIn('Geo3', Fetcher.legacy_names())
        self.assertNotIn('LiChaoTree', Fetcher.legacy_names())
        script = Fetcher.standalone()
        namespace = {'__name__': 'copied_installer_test'}
        exec(compile(script['code'], '<copied-installer>', 'exec'), namespace)
        payload = json.loads(zlib.decompress(base64.b64decode(namespace['PAYLOAD'])))
        self.assertEqual(payload['snippets'], snippets)
        self.assertNotIn('Geo3', payload['snippets'])
        self.assertIn('Geo3', payload['legacyNames'])
        self.assertTrue(all('/extra/' not in str(p) for p in script['sources']))

    def test_both_documents_cover_exactly_their_collection(self):
        for collection in ('core', 'extra'):
            built = Library.build(collection=collection)
            self.assertEqual(built['provenance']['templates'], len(Catalog.catalog(collection=collection)))
            self.assertEqual(built['provenance']['collection'], collection)
            self.assertNotIn('<!-- @readme ', built['markdown'])
            self.assertNotIn('<!-- @code ', built['markdown'])
            self.assertIn('Manifest.json', built['provenance']['sourceSha256'])
        text = config.EXTRA_DOCUMENT.read_text()
        self.assertNotIn('@tool SnippetInstaller', text)
        for deleted in ('StringHash With Node', 'pbdsTree', 'TreeHandler', 'StaticTreePre', 'RingTreeForestPre', '3D ConvexHull', 'stacktrace'):
            self.assertNotIn(deleted, text)

    def test_document_directives_belong_to_their_module_section(self):
        for collection, path in (('core', config.DOCUMENT), ('extra', config.EXTRA_DOCUMENT)):
            modules = Catalog.manifest(collection)
            aliases = {'ZFunction': 'Z-Function', 'DebugerHeadCopyToBitsStdC++': 'DebugerCore'} if collection == 'core' else {}
            sections = []
            code_positions, installer_positions = [], []
            source = path.read_text()
            self.assertNotIn('@readme', source)
            tokens = Library.markdown_parser().parse(source)
            for i, token in enumerate(tokens):
                if token.type == 'heading_open':
                    level, title = int(token.tag[1]), tokens[i + 1].content.strip()
                    sections = [(n, s) for n, s in sections if n < level] + [(level, title)]
                elif token.type == 'html_block':
                    words = token.content.strip().split()
                    if words == ['<!--', '@tool', 'SnippetInstaller', '-->']:
                        installer_positions.append(i)
                    if len(words) != 4 or words[1] != '@code':
                        continue
                    key = words[2]
                    name, *layers = key.split('/')
                    with self.subTest(collection=collection, key=key):
                        self.assertIn(name, modules)
                        expected = [aliases.get(name, name)] + layers
                        self.assertEqual([title for _, title in sections[-len(expected):]], expected)
                        code_positions.append(i)
            if collection == 'core':
                self.assertEqual(len(installer_positions), 1)
                self.assertTrue(code_positions)
                self.assertLess(max(code_positions), installer_positions[0])
            else:
                self.assertEqual(installer_positions, [])

    def test_code_directives_reject_cross_collection_and_duplicate_cards(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'Example.md'
            for collection, other in (('core', 'extra'), ('extra', 'core')):
                key = next(iter(Catalog.catalog(collection=other)))
                path.write_text('<!-- @code ' + key + ' -->\n')
                with self.subTest(collection=collection, key=key), self.assertRaisesRegex(ValueError, '找不到模板'):
                    Library.build(path, {}, require_all=False, collection=collection)
                key = next(iter(Catalog.catalog(collection=collection)))
                path.write_text(('<!-- @code ' + key + ' -->\n\n') * 2)
                with self.subTest(collection=collection, key=key), self.assertRaisesRegex(ValueError, '重复模板引用'):
                    Library.build(path, {}, require_all=False, collection=collection)

    def test_readme_directives_inside_examples_stay_literal(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'Example.md'
            path.write_text('```text\n<!-- @readme Missing -->\n```\n')
            value = Library.build(path, {}, require_all=False)
            self.assertIn('<!-- @readme Missing -->', value['markdown'])
            path.write_text('<!-- @tool SnippetInstaller -->\n')
            with self.assertRaisesRegex(ValueError, 'extra'):
                Library.build(path, {}, require_all=False, collection='extra')

    def test_cli_collection_selection_is_scoped_and_restored(self):
        previous = config.TEST_MODULES, config.TEST_COLLECTION
        seen = []
        def inspect(console, right, perf):
            seen.append((set(config.TEST_MODULES), config.TEST_COLLECTION))
            return 0
        with patch.object(Ctl, 'test', side_effect=inspect), patch.object(Ctl, 'test_lock', return_value=contextlib.nullcontext()), contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(Ctl.main(['test', '--all', '--collection', 'extra']), 0)
        self.assertEqual(seen, [(set(Catalog.manifest('extra', dependencies=True)), 'extra')])
        self.assertEqual((config.TEST_MODULES, config.TEST_COLLECTION), previous)
        with patch.object(Ctl, 'update_local') as install, contextlib.redirect_stdout(io.StringIO()):
            self.assertNotEqual(Ctl.main(['update', '--local', '--collection', 'extra']), 0)
            install.assert_not_called()

    def test_extra_client_target_is_allowed_but_unconfigured_target_is_rejected(self):
        from tools.Yuque import YuqueClient, YuqueError
        client = object.__new__(YuqueClient)
        base = dict(id=7, _yuque=dict(revision='old'))
        client.get = Mock(return_value=base)
        client._run = Mock()
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'body.md'
            path.write_text('# invalid non-Lake document')
            with self.assertRaisesRegex(YuqueError, 'Lake'):
                client.update(config.YUQUE_EXTRA_DOCUMENT, path, base)
        client.get.assert_called_once_with(config.YUQUE_EXTRA_DOCUMENT)
        client._run.assert_not_called()
        client.get.reset_mock()
        with self.assertRaisesRegex(YuqueError, '目标'):
            client.update('owner/book/unconfigured', 'missing', base)
        client.get.assert_not_called()

    def test_publish_preparation_isolates_target_and_files(self):
        client = Mock()
        def get(ref):
            return dict(body_lake='old', body='old', _yuque=dict(revision='revision'))
        client.get.side_effect = get
        result = dict(lake='<!doctype lake>new', markdown='new', provenance={})
        with tempfile.TemporaryDirectory() as tmp, patch.object(config, 'GENERATED', Path(tmp) / 'generated'), patch.object(Publisher, 'STATE', Path(tmp) / 'state'), patch.object(config, 'YUQUE_EXPECTED_REVISION', 'revision'), patch.object(config, 'YUQUE_EXTRA_EXPECTED_REVISION', 'revision'):
            a, _ = Publisher.prepare(client, result, 'core')
            b, _ = Publisher.prepare(client, result, 'extra')
            self.assertEqual(a['ref'], config.YUQUE_DOCUMENT)
            self.assertEqual(b['ref'], config.YUQUE_EXTRA_DOCUMENT)
            self.assertNotEqual(a['prepared'], b['prepared'])
            self.assertTrue((Path(tmp) / 'state/publish/core/base.json').exists())
            self.assertTrue((Path(tmp) / 'state/publish/extra/base.json').exists())
            self.assertNotEqual(Publisher.state_path(a['ref']), Publisher.state_path(b['ref']))


if __name__ == '__main__':
    unittest.main()
