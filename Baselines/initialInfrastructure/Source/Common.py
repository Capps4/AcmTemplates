"""Shared compiler profiles, source fingerprints and bounded process execution."""
import hashlib
import functools
import platform
import json
import os
import re
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent
CXX = os.environ.get('CXX', '/opt/homebrew/bin/g++-15')
SAN_CXX = os.environ.get('SAN_CXX', '/usr/bin/clang++')
FLAGS = {
    'strict': ['-std=gnu++17', '-Wall', '-Wextra', '-Werror', '-Weffc++',
               '-O0', '-g', '-rdynamic', '-fno-omit-frame-pointer'],
    'optimized': ['-std=gnu++17', '-O2', '-Wall', '-Wextra'],
    'sanitized': ['-std=gnu++17', '-O1', '-g', '-fsanitize=address,undefined',
                  '-fno-omit-frame-pointer'],
    'benchmark': ['-std=gnu++17', '-O2'],
}


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def write_json(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(path.suffix + '.tmp')
    tmp.write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n')
    tmp.replace(path)


def manifest():
    return {item['module']: item for item in json.loads((ROOT / 'Manifest.json').read_text())}


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
                                capture_output=True, env=cfg, timeout=timeout)
    except subprocess.TimeoutExpired as exc:
        raise RuntimeError(f'Timeout ({timeout}s): ' + ' '.join(map(str, args))) from exc
    if result.returncode:
        raise RuntimeError(' '.join(map(str, args)) + '\n' + result.stdout + result.stderr)
    return result


def dependencies(paths):
    """Track quoted includes, including inactive alternatives, without editing sources."""
    seen = set()
    def visit(path):
        path = Path(path).resolve()
        if path in seen:
            return
        if not path.is_file():
            raise RuntimeError('Missing input: ' + str(path))
        seen.add(path)
        for name in re.findall(r'^\s*#\s*include\s*"([^"]+)"', path.read_text(), re.MULTILINE):
            visit(path.parent / name)
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
    return [ROOT / key / 'Final.hpp' for key in sorted(seen)]


def fingerprint(paths):
    return {str(path.relative_to(ROOT)): digest(path) for path in dependencies(paths)}


def header_body(name):
    text = (ROOT / name / 'Final.hpp').read_text()
    marker = '// SNIPPET BEGIN\n'
    return text.split(marker, 1)[1] if marker in text else text.replace('#pragma once\n', '')
