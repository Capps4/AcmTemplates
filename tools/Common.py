"""Shared compiler profiles, source fingerprints and bounded process execution."""
import hashlib
import functools
import platform
import json
import os
import re
import shutil
import subprocess
import signal
import tempfile

from pathlib import Path

import tools as config

from tools.Catalog import ROOT, manifest, module_path, catalog
from tools import CXX, SAN_CXX, FLAGS, SOURCE


def digest(path):
    path = Path(path)
    stat = path.stat()
    return file_digest(path, stat.st_mtime_ns, stat.st_ctime_ns, stat.st_size, stat.st_ino, stat.st_dev)


@functools.lru_cache(maxsize=8)
def digest_cache(folder):
    try:
        data = json.loads((folder / 'compiled/Inputs.json').read_text())
        return data if isinstance(data, dict) else {}
    except (OSError, ValueError):
        return {}


@functools.lru_cache(maxsize=8192)
def file_digest(path, *stamp):
    cache = digest_cache(config.CACHE)
    key = str(path.absolute())
    previous = cache.get(key)
    if (isinstance(previous, list) and len(previous) == 2 and previous[0] == list(stamp) and
            isinstance(previous[1], str) and re.fullmatch(r'[0-9a-f]{64}', previous[1])):
        return previous[1]
    value = hashlib.sha256(path.read_bytes()).hexdigest()
    cache[key] = [list(stamp), value]
    while len(cache) > 4096:
        del cache[next(iter(cache))]
    return value


def save_digest_cache():
    cache = digest_cache(config.CACHE)
    if cache:
        write_json(config.CACHE / 'compiled/Inputs.json', cache)


def include_digests(records):
    result = {}
    for path in {p for record in records for p in record.get('actualIncludes', {})}:
        try:
            result[path] = digest(ROOT / path)
        except OSError:
            result[path] = None
    return result


def includes_current(includes, digests=None):
    for path, expected in includes.items():
        value = digests[path] if digests is not None and path in digests else digest(ROOT / path)
        if value != expected:
            return False
    return True



def cached_program(label, signature, actual_digests=None):
    folder = config.CACHE / 'compiled' / hashlib.sha256(label.encode()).hexdigest()
    try:
        data = json.loads((folder / 'Build.json').read_text())
        target = folder / 'program'
        if (data['signature'] == signature and digest(target) == data['binarySha256'] and
                os.access(target, os.X_OK) and
                includes_current(data['actualIncludes'], actual_digests)):
            return target, data['actualIncludes']
    except (OSError, ValueError, KeyError, TypeError):
        pass
    return None


def cache_program(label, signature, target, actual_includes):
    # One current build per job; old versions never accumulate.
    folder = config.CACHE / 'compiled' / hashlib.sha256(label.encode()).hexdigest()
    folder.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='test-build-', dir=config.CACHE) as tmp:
        binary = Path(tmp) / 'program'
        shutil.copy2(target, binary)
        binary.replace(folder / 'program')
    write_json(folder / 'Build.json', {'signature': signature,
               'binarySha256': digest(folder / 'program'), 'actualIncludes': actual_includes})
    return folder / 'program'



def write_json(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(path.suffix + '.tmp')
    tmp.write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n')
    tmp.replace(path)


def test_path(name, modules=None):
    return ROOT / 'tests/Correctness' / module_path(name, modules).relative_to(SOURCE)


def benchmark_path(name, modules=None):
    return ROOT / 'tests/Performance' / module_path(name, modules).relative_to(SOURCE)


@functools.lru_cache(maxsize=None)
def compiler(profile):
    name = SAN_CXX if profile == 'sanitized' else CXX
    path = shutil.which(name)
    if not path:
        raise RuntimeError('Compiler not found: ' + name)
    result = execute([path, '--version'])
    return {'path': str(Path(path).resolve()), 'version': result.stdout.splitlines()[0],
            'target': execute([path, '-dumpmachine']).stdout.strip()}


@functools.lru_cache(maxsize=1)
def hardware():
    data = {'platform': platform.platform(), 'machine': platform.machine(), 'cpu': platform.processor()}
    if sysctl := shutil.which('sysctl'):
        for field, key in [('cpu', 'machdep.cpu.brand_string'), ('model', 'hw.model'), ('memoryBytes', 'hw.memsize')]:
            try:
                data[field] = execute([sysctl, '-n', key]).stdout.strip()
            except RuntimeError:
                pass
    return data


def execute(args, input_text=None, timeout=180, env=None):
    cfg = dict(os.environ, UBSAN_OPTIONS='halt_on_error=1', ASAN_OPTIONS='halt_on_error=1')
    if env:
        cfg.update(env)
    try:
        result = subprocess.run(list(map(str, args)), input=input_text, text=True,
                                capture_output=True, env=cfg, timeout=timeout,
                                stdin=subprocess.DEVNULL if input_text is None else None)
    except subprocess.TimeoutExpired as exc:
        def decode(value):
            return value.decode(errors='replace') if isinstance(value, bytes) else value or ''
        raise ProcessError(args, decode(exc.stdout), decode(exc.stderr), timeout=timeout) from exc
    if result.returncode:
        raise ProcessError(args, result.stdout, result.stderr, result.returncode)
    return result


class ProcessError(RuntimeError):
    """Keep partial output and termination details for case-level diagnostics."""
    def __init__(self, command, stdout='', stderr='', returncode=None, timeout=None):
        self.command = list(map(str, command))
        self.stdout, self.stderr = stdout, stderr
        self.returncode, self.timeout = returncode, timeout
        if timeout is not None:
            reason = f'Timeout ({timeout}s)'
        elif returncode is not None and returncode < 0:
            try:
                reason = 'Signal ' + signal.Signals(-returncode).name
            except ValueError:
                reason = 'Signal ' + str(-returncode)
        else:
            reason = f'Exit {returncode}'
        super().__init__(reason + ': ' + ' '.join(self.command) + '\n' + stdout + stderr)


def content_fingerprint(paths):
    """Hash an inventory without interpreting unrelated translation units."""
    result = {}
    for value in sorted(set(paths)):
        path = Path(value).absolute()
        result[str(path.relative_to(ROOT))] = (
            {'sha256': digest(path), 'link': os.readlink(path)} if path.is_symlink() else digest(path))
    return result


def snapshot(paths, inventory=()):
    values = content_fingerprint(inventory)
    values.update(fingerprint(paths))
    return hashlib.sha256(json.dumps(values, sort_keys=True).encode()).hexdigest()


@functools.lru_cache(maxsize=8192)
def quoted_includes(path, *stamp):
    return tuple(path.parent / name for name in re.findall(
        r'^\s*#\s*include\s*"([^\"]+)"', path.read_text(), re.MULTILINE))


def dependencies(paths):
    """Track quoted includes, including inactive alternatives, without editing sources."""
    seen = set()
    def visit(path):
        path = Path(path)
        path = path.resolve()
        if path in seen:
            return
        if not path.is_file():
            raise RuntimeError('Missing input: ' + str(path))
        seen.add(path)
        if path.suffix.lower() not in ('.c', '.cc', '.cpp', '.cxx', '.h', '.hpp', '.hxx'):
            return
        stat = path.stat()
        for child in quoted_includes(path, stat.st_mtime_ns, stat.st_ctime_ns, stat.st_size, stat.st_ino):
            visit(child)
    for path in paths:
        visit(path)
    return sorted(seen)


def headers(name, modules=None):
    modules = modules or manifest()
    seen = set()
    def visit(key):
        if key in seen:
            return
        seen.add(key)
        for dep in modules[key]['dependencies']:
            visit(dep)
    visit(name)
    return [item['path'] for item in catalog({key: item for key, item in modules.items() if key in seen}).values()]


def fingerprint(paths):
    return {str(path.relative_to(ROOT)): digest(path) for path in dependencies(paths)}
