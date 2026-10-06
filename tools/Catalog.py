"""Repository template catalog and source extraction; no external credentials."""
import json
from pathlib import Path
import re

from tools import ROOT, SOURCE, MANIFEST
ALIASES = {'SparseTable': 'RMQ', 'Fenwick': 'FenwickTree', 'Z-Function': 'ZFunction',
           'geo3': 'Geo3', 'debuger': 'Debuger'}


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


def catalog():
    result = {}
    for name, item in manifest().items():
        folder = module_path(name)
        for layer in item.get('layers', ['code']):
            key = name if layer == 'code' else name + '/' + layer
            result[key] = {'path': folder / (layer + '.hpp'), 'module': name,
                           'name': name if layer == 'code' else layer,
                           'dependencies': item['dependencies']}
    return result


def code_of(path):
    text = Path(path).read_text(encoding='utf-8')
    if '// SNIPPET BEGIN\n' in text:
        text = text.split('// SNIPPET BEGIN\n', 1)[1].split('// SNIPPET END', 1)[0]
    # Strip repository plumbing only. Algorithm macros and system includes stay.
    text = re.sub(r'^\s*#\s*pragma\s+once[^\n]*\n', '', text, flags=re.M)
    text = re.sub(r'^\s*#\s*include\s*"[^"\n]+"[^\n]*\n', '', text, flags=re.M)
    return text.strip() + '\n'


def header_body(name):
    text = (module_path(name) / 'code.hpp').read_text(encoding='utf-8')
    marker = '// SNIPPET BEGIN\n'
    return text.split(marker, 1)[1] if marker in text else text.replace('#pragma once\n', '')


