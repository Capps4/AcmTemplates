"""Enable the preserved Original comment switches only in a generated test source."""
import re
from Common import ROOT, header_body

MODULES = [
    'DisjointSetUnion', 'AcAutomaton', 'CentroidDecomposition', 'ExSuffixAutomaton',
    'SuffixAutomaton', 'StronglyConnectedComponent', 'RingTree', 'LinearBasis',
    'FastFourierTransform',
]


def source():
    headers = set()
    for name in MODULES:
        text = (ROOT / name / 'Final.hpp').read_text()
        headers.update(re.findall(r'^#include <([^>]+)>', text, re.MULTILINE))
    headers.update(['cassert', 'iostream'])
    parts = ['\n'.join('#include <' + h + '>' for h in sorted(headers))]
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
                lines[i] = z[1] + z[2]
            elif z and z[2].strip() == '}':
                lines[i] = z[1] + '}'
        text = '\n'.join(lines)
        if m == 'LinearBasis':
            text = text.replace('k += !canZero;', '')
        parts.append(text)
    parts.append('''
    int main() {
        DSU d(3); d.Union(0,1); d.Union(2,1); assert(d.find(2)==d.find(0));
        AcAutomaton<26,'a'> ac({"a","a","ab"}); assert(ac.ID[ac.terminal[0]]==std::vector<int>({0,1}));
        std::vector<std::vector<int>> g{{1},{0,2,3},{1},{1,4},{3}};
        CentroidDecomposition cd(g); int edges=0; for(auto &v:cd.cdt)edges+=int(v.size()); assert(edges==4);
        ExSam<26,'a'> es({"ab","ac"}); int p=es.son[0]['a'-'a']; assert(es.ID[p].count(0)&&es.ID[p].count(1));
        Sam<26,'a'> sam("aaa"); long long sum=0; for(auto v:sam.cnt)sum+=v; assert(sum==3);
        Scc sc(std::vector<std::vector<int>>{{1,1},{}}); assert(sc.g[sc.bel[0]].size()==1);
        RingTree rt(std::vector<int>{1,2,2}); assert(rt.dsu.find(0)==rt.dsu.find(2));
        LinearBasis<int> lb; lb.insert(1); assert(lb.findByOrder(0)==0&&lb.findByOrder(1)==1);
        FftPoly a(129,1), c=a*a; assert(c[128]==129&&c[256]==1);
        std::cout << "Original commented options enabled: compile and behavior PASS\\n";
    }
    ''')
    return "\n\n".join(parts)
