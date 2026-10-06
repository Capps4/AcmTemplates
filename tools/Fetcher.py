"""Generate repository snippets and package the shared standalone installer."""
import base64
import json
import textwrap
import zlib

from tools.Catalog import catalog, code_of
from tools import SnippetInstaller as installer
from tools.SnippetInstaller import OWNER, candidates, load, prefixes, shutil
from tools import ROOT, MANIFEST, SNIPPETS_FILE as FILENAME, CACHE


def snippets():
    result = {}
    for key, item in catalog().items():
        prefixes = item['prefixes']
        # TextMate escaping first; JSON escaping is handled by json.dumps.
        body = code_of(item['path']).rstrip('\n').replace('\\', '\\\\').replace('$', '\\$').replace('}', '\\}')
        result[key] = {'scope': 'cpp', 'prefix': prefixes if len(prefixes) > 1 else prefixes[0],
                       'body': body.splitlines() + ['$0'], 'description': OWNER + key}
    return result


def legacy_names():
    items = list(catalog().values())
    return {item['name'] for item in items} | {alias for item in items for alias in item['aliases']}


def locate(directory=None, user_data=None):
    paths = candidates() if not directory and not user_data else None
    return installer.locate(directory, user_data, paths)


def legacy(path, data):
    return installer.legacy(path, data, legacy_names())


def plan(directory):
    return installer.plan(directory, snippets(), legacy_names(), FILENAME)


def install(directory, migrate=False):
    return installer.install(directory, snippets(), legacy_names(), FILENAME,
                             migrate=migrate, backup_root=CACHE / 'snippet-backups')


def standalone():
    """Embed the same installation source and current data, without local settings."""
    path = ROOT / 'tools/SnippetInstaller.py'
    payload = {'snippets': snippets(), 'legacyNames': sorted(legacy_names()), 'filename': FILENAME}
    packed = base64.b64encode(zlib.compress(json.dumps(
        payload, ensure_ascii=False, separators=(',', ':')).encode('utf-8'), 9)).decode('ascii')
    literals = '\n'.join('    ' + repr(line) for line in textwrap.wrap(packed, 88))
    code = (path.read_text(encoding='utf-8').rstrip() +
            "\n\n# 内置模板数据，由仓库生成；更新时请重新复制整段脚本。\n" +
            "import base64\nimport zlib\n\nPAYLOAD = (\n" + literals + "\n)\n\n" +
            "if __name__ == '__main__':\n" +
            "    payload = json.loads(zlib.decompress(base64.b64decode(PAYLOAD)).decode('utf-8'))\n" +
            "    sys.exit(main(payload['snippets'], payload['legacyNames'], payload['filename']))\n")
    sources = [path, ROOT / 'tools/Fetcher.py', ROOT / 'tools/Catalog.py',
               ROOT / 'tools/__init__.py', MANIFEST]
    sources.extend(item['path'] for item in catalog().values())
    return {'code': code, 'sources': sources}
