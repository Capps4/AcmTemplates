"""Internal Yuque client for ctl update --yuque; never prints credentials."""
import getpass
import hashlib
import json
import os
from pathlib import Path
import secrets
import shutil
import subprocess
import sys
from urllib.parse import unquote, urlsplit

import tools as config
from tools import YUQUE_STATE as STATE, YUQUE_TOKEN as TOKEN, YUQUE_CLI as CLI
from tools.Common import write_json


class YuqueError(Exception):
    def __init__(self, message, code=1):
        super().__init__(message)
        self.code = code


def parse_ref(ref):
    if '://' in ref:
        url = urlsplit(ref)
        if url.scheme != 'https' or url.hostname != 'www.yuque.com' or url.port:
            raise YuqueError('只支持 https://www.yuque.com 文档链接。', 2)
        path = url.path
    else:
        path = ref.split('?', 1)[0].split('#', 1)[0]
    parts = [unquote(part) for part in path.strip('/').split('/')]
    if len(parts) != 3 or any(not part or part in ('.', '..') or '/' in part
                              or part.startswith('-') for part in parts):
        raise YuqueError('请在 tools/__init__.py 配置 owner/book/doc 文档地址。', 2)
    return '/'.join(parts[:2]), parts[2]


def read_token():
    value = os.environ.get('YUQUE_TOKEN')
    if value:
        return value
    if not TOKEN.exists():
        return None
    if TOKEN.stat().st_mode & 0o077:
        raise YuqueError('Token 文件权限过宽，请运行 chmod 600 ' + str(TOKEN), 3)
    return TOKEN.read_text(encoding='utf-8').strip()


def save_token(value):
    TOKEN.parent.mkdir(mode=0o700, parents=True, exist_ok=True)
    os.chmod(TOKEN.parent, 0o700)
    temporary = TOKEN.parent / ('token-' + secrets.token_hex(6) + '.tmp')
    try:
        with os.fdopen(os.open(temporary, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600), 'w') as stream:
            stream.write(value + '\n')
        temporary.replace(TOKEN)
    finally:
        temporary.unlink(missing_ok=True)


def revision(doc):
    fields = ('id', 'slug', 'title', 'format', 'body', 'body_lake', 'body_draft',
              'content_updated_at', 'published_at')
    value = {key: doc.get(key) for key in fields}
    return hashlib.sha256(json.dumps(value, sort_keys=True, ensure_ascii=False).encode()).hexdigest()


def body_of(doc, format='lake'):
    field = {'markdown': 'body', 'lake': 'body_lake'}[format]
    value = doc.get(field)
    if not isinstance(value, str):
        raise YuqueError('API 未返回 ' + field + '，不能将空正文回写。')
    return value


class YuqueClient:
    def __init__(self, token=None):
        self.token = token or read_token()
        if not self.token:
            raise YuqueError('尚未配置语雀 Token；ctl update --local 和测试不需要 Token。'
                             '请配置 YUQUE_TOKEN 环境变量，或在终端运行 ctl update --yuque 隐藏输入。', 3)
        self.node = shutil.which('node')
        if not self.node or not CLI.exists():
            raise YuqueError('缺少 Node 或官方 CLI；安装 Node.js 20+ 后运行 ctl update --yuque。', 2)

    def _run(self, *args):
        env = os.environ.copy()
        env.update(YUQUE_TOKEN=self.token, YUQUE_HOST='https://www.yuque.com', YUQUE_TIMEOUT_MS='30000')
        try:
            result = subprocess.run([self.node, str(CLI), '--json', *map(str, args)],
                                    env=env, capture_output=True, text=True, timeout=config.YUQUE_REQUEST_TIMEOUT)
        except subprocess.TimeoutExpired:
            raise YuqueError('请求超时；写入可能已经生效，请先检查远端和本地备份，勿直接重试。', 6) from None
        if result.returncode:
            message = (result.stderr or '语雀 CLI 请求失败。').strip().replace(self.token, '[REDACTED]')
            raise YuqueError(message, result.returncode if 0 < result.returncode < 256 else 1)
        try:
            return json.loads(result.stdout)
        except json.JSONDecodeError:
            raise YuqueError('官方 CLI 未返回有效 JSON。') from None

    def status(self):
        return self._run('auth', 'status')

    def get(self, ref):
        book, doc = parse_ref(ref)
        data = self._run('doc', 'get', book, doc)
        data['_yuque'] = {'ref': book + '/' + str(data.get('slug') or doc), 'revision': revision(data)}
        return data

    def update(self, ref, body_file, base):
        # Only the configured target is writable; callers cannot pass an arbitrary page.
        if parse_ref(ref) != parse_ref(config.YUQUE_DOCUMENT) or not base:
            raise YuqueError('发布目标必须匹配 tools/__init__.py，并提供远端快照。', 2)
        current = self.get(ref)
        if base.get('id') != current['id'] or base.get('_yuque', {}).get('revision') != current['_yuque']['revision']:
            raise YuqueError('文档已变化或快照不匹配，未执行写入。', 6)
        file = Path(body_file).resolve(strict=True)
        expected = file.read_text(encoding='utf-8')
        if not expected.lstrip().lower().startswith('<!doctype lake>'):
            raise YuqueError('发布正文必须是生成的 Lake 格式。', 2)
        latest = self.get(ref)
        if latest['_yuque']['revision'] != current['_yuque']['revision']:
            raise YuqueError('写入前发现远端变化，未执行写入。', 6)
        backup = STATE / 'backups' / str(current['id'])
        backup.mkdir(parents=True, exist_ok=True, mode=0o700)
        backup_file = backup / (current['_yuque']['revision'] + '.json')
        write_json(backup_file, current)
        write_json(backup / 'latest.json', current)
        book, _ = parse_ref(ref)
        self._run('doc', 'update', book, str(current['id']), '--format', 'lake', '--body-file', file)
        result = self.get(ref)
        verified = body_of(result).replace('\r\n', '\n').rstrip('\n') == expected.replace('\r\n', '\n').rstrip('\n')
        result['_yuque'].update(verified=verified, backup=str(backup_file))
        return result


def ensure_client(console):
    """Authentication setup is part of update, not a separate public command."""
    token = read_token()
    entered = False
    if not token:
        if not config.YUQUE_PROMPT_TOKEN or not sys.stdin.isatty():
            raise YuqueError('缺少语雀 Token。请在 https://www.yuque.com/settings/tokens 创建，'
                             '配置 YUQUE_TOKEN，或在终端运行 ctl update --yuque 隐藏输入。', 3)
        console.note('在 https://www.yuque.com/settings/tokens 创建 Token；输入隐藏，不会写入 Git。')
        token = getpass.getpass('语雀 Token：').strip()
        entered = True
    if not token:
        raise YuqueError('Token 不能为空。', 3)
    node, npm = shutil.which('node'), shutil.which('npm')
    if not node:
        raise YuqueError('语雀更新需要 Node.js 20+；请先安装 Node.js。', 2)
    major = int(subprocess.check_output([node, '--version'], text=True).strip().lstrip('v').split('.')[0])
    if major < 20:
        raise YuqueError('语雀更新需要 Node.js 20+。', 2)
    if not CLI.exists():
        if not npm:
            raise YuqueError('安装官方语雀 CLI 需要 npm。', 2)
        console.note('安装语雀官方 CLI ' + config.YUQUE_CLI_VERSION + ' 到 .yuque/runtime/')
        env = os.environ.copy()
        env.pop('YUQUE_TOKEN', None);env.pop('YUQUE_PERSONAL_TOKEN', None)
        result = subprocess.run([npm, 'install', '--prefix=' + str(STATE / 'runtime'),
                                 '--cache=' + str(STATE / 'npm-cache'), '--registry=' + config.YUQUE_REGISTRY,
                                 '--ignore-scripts', '--no-audit', '--no-fund',
                                 'yuque-open-cli@' + config.YUQUE_CLI_VERSION],
                                env=env, capture_output=True, text=True, timeout=180)
        if result.returncode:
            raise YuqueError('官方 CLI 安装失败，请检查网络。\n' + result.stderr.replace(token, '[REDACTED]'), 2)
    client = YuqueClient(token)
    client.status()
    if entered:
        save_token(token)
    console.note('语雀认证通过')
    return client
