"""Repository template catalog and source extraction; no external credentials."""
import json
import os
from pathlib import Path
import re

from tools import ROOT, SOURCE, MANIFEST


def manifest():
    data = json.loads(MANIFEST.read_text(encoding='utf-8'))
    result = {item['module']: item for item in data}
    if len(result) != len(data):
        raise ValueError('Manifest contains duplicate modules')
    return result


def module_path(name, modules=None):
    modules = manifest() if modules is None else modules
    path = (ROOT / modules[name]['path']).resolve()
    path.relative_to(SOURCE)
    return path


def catalog(modules=None):
    modules = manifest() if modules is None else modules
    result = {}
    for name, item in modules.items():
        folder = module_path(name, modules)
        previous = ROOT / item['include'] if item.get('include') else ROOT / 'Headers/Headers.hpp'
        for layer in item.get('layers', ['code']):
            key = name if layer == 'code' else name + '/' + layer
            path = folder / (layer + '.hpp')
            short = name if layer == 'code' else layer
            result[key] = {'path': path, 'module': name,
                           'name': short,
                           'dependencies': item['dependencies'],
                           'include': os.path.relpath(previous, folder),
                           'prefixes': item.get('prefixes', ['_T_' + short]),
                           'aliases': item.get('aliases', [])}
            previous = path
    return result


def code_of(path):
    text = Path(path).read_text(encoding='utf-8')
    if '// SNIPPET BEGIN\n' in text:
        text = text.split('// SNIPPET BEGIN\n', 1)[1].split('// SNIPPET END', 1)[0]
    # Strip repository plumbing only. Algorithm macros and system includes stay.
    text = re.sub(r'^\s*#\s*pragma\s+once[^\n]*\n', '', text, flags=re.M)
    text = re.sub(r'^\s*#\s*include\s*"[^"\n]+"[^\n]*\n', '', text, flags=re.M)
    return text.strip() + '\n'
