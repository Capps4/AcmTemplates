"""Generate VS Code snippets from local sources and install only managed files."""
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import platform
import re
import secrets
import shutil
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.Catalog import ALIASES, catalog, code_of
from tools.Common import write_json

from tools import SNIPPETS_FILE as FILENAME, CACHE
OWNER = 'AcmTemplates generated: '


def snippets():
    result = {}
    for key, item in catalog().items():
        name = item['name']
        prefixes = ['_T_' + name]
        if name == 'RMQ':
            prefixes.append('_T_SparseTable')
        # TextMate escaping first; JSON escaping is handled by json.dumps.
        body = code_of(item['path']).rstrip('\n').replace('\\', '\\\\').replace('$', '\\$').replace('}', '\\}')
        result[key] = {'scope': 'cpp', 'prefix': prefixes if len(prefixes) > 1 else prefixes[0],
                       'body': body.splitlines() + ['$0'], 'description': OWNER + key}
    return result


def candidates(home=None, system=None, env=None):
    """Find installed user configurations, including profiles/portable/server data."""
    home = Path.home() if home is None else Path(home)
    system = platform.system() if system is None else system
    env = os.environ if env is None else env
    users = []
    if env.get('VSCODE_PORTABLE'):
        users.append(Path(env['VSCODE_PORTABLE']) / 'user-data/User')
    # A portable installation can be discovered beside a code executable on PATH.
    for executable in ('code', 'code-insiders'):
        location = shutil.which(executable)
        if not location:
            continue
        binary = Path(location).resolve()
        if system == 'Darwin':
            app = next((p for p in binary.parents if p.suffix == '.app'), None)
            if app:
                folder = 'code-insiders-portable-data' if executable.endswith('insiders') else 'code-portable-data'
                users.append(app.parent / folder / 'user-data/User')
        else:
            root = binary.parent.parent if binary.parent.name == 'bin' else binary.parent
            users.append(root / 'data/user-data/User')
    if system == 'Darwin':
        base = home / 'Library/Application Support'
    elif system == 'Windows':
        base = Path(env.get('APPDATA', str(home / 'AppData/Roaming')))
    else:
        base = Path(env.get('XDG_CONFIG_HOME', str(home / '.config')))
    users.extend(base / name / 'User' for name in ('Code', 'Code - Insiders'))
    if system == 'Linux':
        users.extend(home / '.var/app' / app / 'config/Code/User'
                     for app in ('com.visualstudio.code', 'com.visualstudio.code.insiders'))
    users.extend(home / name / 'data/User' for name in ('.vscode-server', '.vscode-server-insiders'))
    # WSL can discover the Windows roaming directory without knowing a username.
    if system == 'Linux' and env.get('WSL_DISTRO_NAME') and shutil.which('cmd.exe') and shutil.which('wslpath'):
        try:
            win = subprocess.check_output(['cmd.exe', '/c', 'echo', '%APPDATA%'], text=True, timeout=5).strip()
            if re.match(r'^[A-Za-z]:\\', win):
                mounted = subprocess.check_output(['wslpath', '-u', win], text=True, timeout=5).strip()
                users.extend(Path(mounted) / name / 'User' for name in ('Code', 'Code - Insiders'))
        except (OSError, subprocess.SubprocessError):
            pass
    result = []
    for user in users:
        if user.is_dir():
            result.append(user / 'snippets')
            profiles = user / 'profiles'
            if profiles.is_dir():
                result.extend(p / 'snippets' for p in sorted(profiles.iterdir()) if p.is_dir())
    return list(dict.fromkeys(path.resolve() for path in result))


def locate(directory=None, user_data=None):
    if directory and user_data:
        raise ValueError('SNIPPETS_DIR 和 VSCODE_USER_DATA 只能配置一个')
    if directory:
        return Path(directory).expanduser().resolve()
    if user_data:
        return (Path(user_data).expanduser() / 'User/snippets').resolve()
    paths = candidates()
    if not paths:
        raise ValueError('未找到 VS Code 用户配置。请启动一次 VS Code，或在 tools/__init__.py 设置 SNIPPETS_DIR。')
    if len(paths) > 1:
        raise ValueError('找到多个 VS Code 配置，请在 tools/__init__.py 设置 SNIPPETS_DIR：\n' + '\n'.join(map(str, paths)))
    return paths[0]


def prefixes(entry):
    value = entry.get('prefix', [])
    return [value] if isinstance(value, str) else value if isinstance(value, list) else []


def load(path):
    # Existing snippets may be JSONC. Never delete a file we cannot safely parse.
    try:
        text = path.read_text(encoding='utf-8-sig')
        strings_comments = re.compile(r'"(?:\\.|[^"\\])*"|//[^\n]*|/\*.*?\*/', re.S)
        text = strings_comments.sub(lambda m: m[0] if m[0].startswith('"') else ' ', text)
        strings_commas = re.compile(r'"(?:\\.|[^"\\])*"|,(?=\s*[}\]])')
        text = strings_commas.sub(lambda m: m[0] if m[0].startswith('"') else '', text)
        value = json.loads(text)
    except (ValueError, UnicodeError):
        raise ValueError('无法安全解析 snippets 文件，请先整理或移走：' + str(path)) from None
    if not isinstance(value, dict) or any(not isinstance(v, dict) for v in value.values()):
        raise ValueError('无效 snippets 对象：' + str(path))
    return value


def legacy(path, data):
    names = {item['name'] for item in catalog().values()} | set(ALIASES)
    if path.stem not in names or set(data) != {'Print to console'}:
        return False
    entry = data['Print to console']
    return (entry.get('prefix') == '_T_' + path.stem and entry.get('scope') == 'cpp'
            and entry.get('description') == 'Log output to console'
            and isinstance(entry.get('body'), list)
            and all(isinstance(line, str) for line in entry['body']))


def plan(directory):
    directory = Path(directory)
    wanted = {p for entry in snippets().values() for p in prefixes(entry)}
    migrate, conflicts = [], []
    files = list(directory.glob('*.code-snippets'))
    if (directory / 'cpp.json').exists():
        files.append(directory / 'cpp.json')
    for path in sorted(files):
        if path.name == FILENAME:
            data = load(path)
            if not data or any(not str(v.get('description', '')).startswith(OWNER) for v in data.values()):
                raise ValueError('目标文件含有非本工具管理的内容，不会覆盖：' + str(path))
            continue
        data = load(path)
        if wanted.intersection(p for v in data.values() for p in prefixes(v)):
            (migrate if legacy(path, data) else conflicts).append(path)
    if conflicts:
        raise ValueError('发现个人或来源不明的重复前缀，请手动处理：\n' + '\n'.join(map(str, conflicts)))
    return migrate


def install(directory, migrate=False):
    directory = Path(directory)
    old = plan(directory)
    if old and not migrate:
        raise ValueError('发现旧模板文件；请在 tools/__init__.py 开启 MIGRATE_LEGACY_SNIPPETS 以备份迁移。')
    data = snippets()
    text = json.dumps(data, ensure_ascii=False, indent=2) + '\n'
    target = directory / FILENAME
    if target.exists() and target.read_text(encoding='utf-8') == text and not old:
        return {'installed': str(target), 'changed': False, 'templates': len(data)}
    directory.mkdir(parents=True, exist_ok=True)
    backup = CACHE / 'snippet-backups' / (datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + secrets.token_hex(4))
    backup.mkdir(parents=True)
    files = old + ([target] if target.exists() else [])
    for path in files:
        shutil.copy2(path, backup / path.name)
    write_json(backup / 'Migration.json', {'directory': str(directory), 'files': [p.name for p in files]})
    # Write atomically before removing old files. On failure restore their copies.
    temporary = directory / (FILENAME + '.' + secrets.token_hex(4) + '.tmp')
    try:
        temporary.write_text(text, encoding='utf-8')
        temporary.replace(target)
        for path in old:
            path.unlink()
    except OSError:
        for path in files:
            shutil.copy2(backup / path.name, path)
        if target not in files:
            target.unlink(missing_ok=True)
        raise
    finally:
        temporary.unlink(missing_ok=True)
    return {'installed': str(target), 'changed': True, 'templates': len(data),
            'migrated': [p.name for p in old], 'backup': str(backup)}

