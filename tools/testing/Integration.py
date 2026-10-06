"""Generate coexistence tests directly from code.hpp."""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools.Common import ROOT, manifest, module_path


def inputs():
    return manifest()


def assemble(modules=None, with_test=True):
    modules = modules or inputs()
    lines = [f'#include "{ROOT / "Headers/Headers.hpp"}"']
    for name, item in sorted(modules.items(), key=lambda item: (item[0] == "FastInputOutput", item[0])):
        # Isolate the public aliases only in the generated coexistence test.
        if name == 'FastFourierTransform':
            lines.extend(['#define Float FftFloat', '#define Poly FftPoly'])
        elif name == 'NumberTheoreticTransform':
            lines.append('#define Poly NttPoly')
        if name in ('SegTree', 'SparseSegTree', 'Trie'):
            lines.append(f'#define Info {name}ExampleInfo')
        if name == 'SegTree':
            lines.append('#define Tag SegExampleTag')
        if name == 'Geo3':
            lines.extend(f'#define {key} Geo3{key}' for key in ('Point', 'Vec', 'Line', 'Seg', 'Hit'))
        lines.append(f'#include "{module_path(name, modules) / ("Circle.hpp" if name == "Geo2" else "code.hpp")}"')
        if name in ('SegTree', 'SparseSegTree', 'Trie'):
            lines.append('#undef Info')
        if name == 'SegTree':
            lines.append('#undef Tag')
        if name == 'Geo3':
            lines.extend(f'#undef {key}' for key in ('Point', 'Vec', 'Line', 'Seg', 'Hit'))
        if name in ('FastFourierTransform', 'NumberTheoreticTransform'):
            lines.append('#undef Poly')
        if name == 'FastFourierTransform':
            lines.append('#undef Float')
    if with_test:
        lines.append(f'#include "{ROOT / "tests/Correctness/Integration/Test.hpp"}"')
    return '\n'.join(lines) + '\n'
