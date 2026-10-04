"""Generate coexistence tests directly from Final.hpp."""
from Common import ROOT, manifest


def inputs():
    return {name: item for name, item in manifest().items() if not item.get('deferred')}


def assemble(modules=None, with_test=True):
    modules = modules or inputs()
    lines = []
    for name, item in sorted(modules.items()):
        if name == 'SegTree':
            lines.extend(['#define Info SegExampleInfo', '#define Tag SegExampleTag'])
        lines.append(f'#include "{ROOT / name / "Final.hpp"}"')
        if name == 'SegTree':
            lines.extend(['#undef Tag', '#undef Info'])
    if with_test:
        lines.append(f'#include "{ROOT / "Integration/Test.hpp"}"')
    return '\n'.join(lines) + '\n'


if __name__ == '__main__':
    from Run import main
    main(['integration'])
