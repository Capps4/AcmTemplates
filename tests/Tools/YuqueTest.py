"""Regression assertions for configured-target and snapshot write guards."""
import copy
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import Mock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools.Yuque import YuqueClient, YuqueError
from CardSupport import decode


class YuqueTest(unittest.TestCase):
    def client(self):
        client = object.__new__(YuqueClient)
        base = {'id': 7, 'slug': 'doc', 'format': 'lake', 'body_lake': '<!doctype lake>old',
                '_yuque': {'revision': 'a', 'ref': 'owner/book/doc'}}
        client.get = Mock(side_effect=lambda ref: copy.deepcopy(base))
        client._run = Mock()
        return client, base

    def test_unknown_encoding_rejected(self):
        with self.assertRaisesRegex(ValueError, 'encoding'):
            decode('<card value="other:data"></card>')

    def test_target_must_be_configured(self):
        client, base = self.client()
        with patch('tools.Yuque.config.YUQUE_DOCUMENT', 'owner/book/other'):
            with self.assertRaisesRegex(YuqueError, '目标'):
                client.update('owner/book/doc', 'missing', base)
        client.get.assert_not_called();client._run.assert_not_called()

    def test_snapshot_required(self):
        client, _ = self.client()
        with patch('tools.Yuque.config.YUQUE_DOCUMENT', 'owner/book/doc'):
            with self.assertRaisesRegex(YuqueError, '快照'):
                client.update('owner/book/doc', 'missing', None)

    def test_changed_revision_rejected_before_file_or_write(self):
        client, base = self.client();base = copy.deepcopy(base);base['_yuque']['revision'] = 'older'
        with patch('tools.Yuque.config.YUQUE_DOCUMENT', 'owner/book/doc'):
            with self.assertRaisesRegex(YuqueError, '变化'):
                client.update('owner/book/doc', 'missing', base)
        client._run.assert_not_called()

    def test_lake_required(self):
        client, base = self.client()
        with tempfile.TemporaryDirectory() as tmp, patch('tools.Yuque.config.YUQUE_DOCUMENT', 'owner/book/doc'):
            file = Path(tmp) / 'doc.md';file.write_text('# title')
            with self.assertRaisesRegex(YuqueError, 'Lake'):
                client.update('owner/book/doc', file, base)
        client._run.assert_not_called()

    def test_race_detected_before_write(self):
        client, base = self.client();changed = copy.deepcopy(base);changed['_yuque']['revision'] = 'b'
        client.get.side_effect = [base, changed]
        with tempfile.TemporaryDirectory() as tmp, patch('tools.Yuque.config.YUQUE_DOCUMENT', 'owner/book/doc'):
            file = Path(tmp) / 'doc.lake';file.write_text('<!doctype lake>new')
            with self.assertRaisesRegex(YuqueError, '写入前'):
                client.update('owner/book/doc', file, base)
        client._run.assert_not_called()


if __name__ == '__main__':
    unittest.main()
