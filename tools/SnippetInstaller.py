"""Standard-library snippet installation shared by CTL and the copied installer."""
import argparse
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

OWNER = 'AcmTemplates generated: '


def candidates(home=None, system=None, env=None):
    """Find user configurations even before their snippets folder is created."""
    home = Path.home() if home is None else Path(home)
    system = platform.system() if system is None else system
    env = os.environ if env is None else env
    users = []
    if env.get('VSCODE_PORTABLE'):
        users.append(Path(env['VSCODE_PORTABLE']) / 'user-data/User')
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


def locate(directory=None, user_data=None, paths=None):
    if directory and user_data:
        raise ValueError('SNIPPETS_DIR 和 VSCODE_USER_DATA 只能配置一个')
    if directory:
        return Path(directory).expanduser().resolve()
    if user_data:
        return (Path(user_data).expanduser() / 'User/snippets').resolve()
    paths = candidates() if paths is None else paths
    if len(paths) == 1:
        return paths[0]
    if not sys.stdin.isatty():
        if not paths:
            raise ValueError('未找到 VS Code 用户配置。请启动一次 VS Code，或指定 SNIPPETS_DIR / --dir。')
        raise ValueError('找到多个 VS Code 配置，请指定 SNIPPETS_DIR / --dir：\n' + '\n'.join(map(str, paths)))
    if paths:
        print('找到多个 VS Code 配置，请选择安装位置：')
        for index, path in enumerate(paths, 1):
            print(str(index) + '. ' + str(path))
        print('0. 输入自定义 snippets 目录')
        while True:
            choice = input('输入编号：').strip()
            if choice == '0':
                break
            if choice.isascii() and choice.isdecimal() and 1 <= int(choice) <= len(paths):
                return paths[int(choice) - 1]
            print('请输入列表中的编号。')
    else:
        print('未找到 VS Code 用户配置，请先启动一次 VS Code，或输入自定义 snippets 目录。')
    directory = input('snippets 目录（留空取消）：').strip()
    if not directory:
        raise ValueError('未指定安装目录，已取消')
    return Path(directory).expanduser().resolve()


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


def legacy(path, data, names):
    if path.stem not in names or set(data) != {'Print to console'}:
        return False
    entry = data['Print to console']
    return (entry.get('prefix') == '_T_' + path.stem and entry.get('scope') == 'cpp'
            and entry.get('description') == 'Log output to console'
            and isinstance(entry.get('body'), list)
            and all(isinstance(line, str) for line in entry['body']))


def plan(directory, data, names, filename):
    directory = Path(directory)
    wanted = {p for entry in data.values() for p in prefixes(entry)}
    migrate, conflicts = [], []
    files = list(directory.glob('*.code-snippets'))
    if (directory / 'cpp.json').exists():
        files.append(directory / 'cpp.json')
    for path in sorted(files):
        if path.name == filename:
            existing = load(path)
            if not existing or any(not str(v.get('description', '')).startswith(OWNER) for v in existing.values()):
                raise ValueError('目标文件含有非本工具管理的内容，不会覆盖：' + str(path))
            continue
        existing = load(path)
        if wanted.intersection(p for v in existing.values() for p in prefixes(v)):
            (migrate if legacy(path, existing, names) else conflicts).append(path)
    if conflicts:
        raise ValueError('发现个人或来源不明的重复前缀，请手动处理：\n' + '\n'.join(map(str, conflicts)))
    return migrate


def install(directory, data, names, filename, migrate=False, backup_root=None):
    directory = Path(directory)
    old = plan(directory, data, names, filename)
    if old and not migrate:
        raise ValueError('发现旧模板文件；请开启 MIGRATE_LEGACY_SNIPPETS 以备份迁移。')
    text = json.dumps(data, ensure_ascii=False, indent=2) + '\n'
    target = directory / filename
    if target.exists() and target.read_text(encoding='utf-8') == text and not old:
        return {'installed': str(target), 'changed': False, 'templates': len(data)}
    directory.mkdir(parents=True, exist_ok=True)
    if backup_root is None:
        if platform.system() == 'Darwin':
            cache = Path.home() / 'Library/Caches'
        elif platform.system() == 'Windows':
            cache = Path(os.environ.get('LOCALAPPDATA', str(Path.home() / 'AppData/Local')))
        else:
            cache = Path(os.environ.get('XDG_CACHE_HOME', str(Path.home() / '.cache')))
        backup_root = cache / 'acm-templates/snippet-backups'
    backup = Path(backup_root) / (datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ') + '-' + secrets.token_hex(4))
    backup.mkdir(parents=True)
    files = old + ([target] if target.exists() else [])
    for path in files:
        shutil.copy2(path, backup / path.name)
    (backup / 'Migration.json').write_text(json.dumps(
        {'directory': str(directory), 'files': [p.name for p in files]}, ensure_ascii=False, indent=2) + '\n',
        encoding='utf-8')
    # Write atomically before removing old files. On failure restore their copies.
    temporary = directory / (filename + '.' + secrets.token_hex(4) + '.tmp')
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


def main(data, names, filename):
    """Entry for the self-contained document script; never reads repository state."""
    parser = argparse.ArgumentParser(description='安装内置的 ACM Templates snippets（Python 3.9+，无需联网）')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--dir', help='指定 snippets 目录')
    group.add_argument('--user-data-dir', help='指定 VS Code 用户数据目录')
    args = parser.parse_args()
    try:
        directory = locate(args.dir, args.user_data_dir)
        result = install(directory, data, names, filename, migrate=True)
        print('安装位置：' + result['installed'])
        if result.get('backup'):
            print('备份位置：' + result['backup'])
        if result.get('migrated'):
            print('已备份迁移：' + '、'.join(result['migrated']))
        print(str(result['templates']) + ' 个模板已安装' if result['changed'] else '模板内容相同，无需写入')
        print('在 C++ 文件中输入 _T_模块名，选择补全项插入模板。')
        return 0
    except (OSError, ValueError, EOFError) as exc:
        print('安装失败：' + str(exc), file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print('\n已取消', file=sys.stderr)
        return 130
