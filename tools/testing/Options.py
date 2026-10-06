"""Enable the preserved code comment switches only in a generated test source."""
import re
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools.Common import ROOT, header_body

MODULES = [
    'DisjointSetUnion', 'CentroidDecomposition', 'StronglyConnectedComponent', 'RingTree', 'LinearBasis',
    'FastFourierTransform',
]


def source():
    parts = [f'#include "{ROOT / "Headers/Headers.hpp"}"']
    starts = (
        'std::vector<', 'DSU dsu', 'cdt.resize', 'std::vector<int> fa', 'if (fa[s]',
        'fa[*it]', 'ID.resize', 'for (int i', 'ID[', 'ID.emplace_back', 'cnt.push_back',
        'cnt[cur]', 'dsu.init', 'dsu.Union', 'if (size[x]', 'std::swap(x, y)',
        'for (auto &edges', 'std::sort(edges', 'edges.erase', 'res[i] = std::round',
    )
    for m in MODULES:
        lines = header_body(m).splitlines()
        for i, line in enumerate(lines):
            z = re.match(r'^(\s*)// (.*)$', line)
            if z and z[2].lstrip().startswith(starts):
                body = z[2]
                if m == 'CentroidDecomposition':
                    # The option's centroid parent must not shadow DFS's fa.
                    body = re.sub(r'\bfa\b', 'cfa', body)
                lines[i] = z[1] + body
            elif z and z[2].strip() == '}':
                lines[i] = z[1] + '}'
        text = '\n'.join(lines)
        if m == 'LinearBasis':
            text = text.replace('k += !canZero;', '')
        parts.append(text)
    parts.append('''
    int main() {
        DSU d(3); d.Union(0,1); d.Union(2,1); assert(d.find(2)==d.find(0));
        std::vector<std::vector<int>> g{{1},{0,2,3},{1},{1,4},{3}};
        CentroidDecomposition cd(g); int edges=0; for(auto &v:cd.cdt)edges+=int(v.size()); assert(edges==4);
        SCC sc(std::vector<std::vector<int>>{{1,1},{}}); assert(sc.g[sc.bel[0]].size()==1);
        RingTree rt(std::vector<int>{1,2,2}); assert(rt.dsu.find(0)==rt.dsu.find(2));
        LinearBasis<int> lb; lb.insert(1); assert(lb.findByOrder(0)==0&&lb.findByOrder(1)==1);
        Poly a(129,1), c=a*a; assert(c[128]==129&&c[256]==1);
        std::cout << "Template commented options enabled: compile and behavior PASS\\n";
    }
    ''')
    return "\n\n".join(parts)
