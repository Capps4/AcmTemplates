from pathlib import Path
import hashlib
import json
import tempfile

report=Path(__file__).resolve().parent
root=report.parents[2]
build=Path(tempfile.mkdtemp(prefix='tree-map-gap-diagnosis-'))
current=(root/'list_helper_cpp17/ListHelperAdaptive.hpp').read_text()
start=current.index('template<int B, bool Descending')
end=current.index('template<class Equal = std::equal_to<>>')
operator=current[current.index('namespace seq {'):start]
core=current[start:end]
base='''#pragma once
#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>
'''
variants={'current':core}
variants['swap']=core.replace('std::move(b.begin(), b.end(), a.begin());','a.swap(b);')
forward=core.replace('''            for (std::size_t j = 1; j < cnt.size(); ++j)
                cnt[j] += cnt[j - 1];
            for (std::size_t j = n; j-- > 0;)
                output[--cnt[(key(first[j]) >> shift) & mask]] = first[j];''','''            std::exclusive_scan(cnt.begin(), cnt.end(), cnt.begin(), std::size_t{0});
            for (std::size_t j = 0; j < n; ++j)
                output[cnt[(key(first[j]) >> shift) & mask]++] = first[j];''')
assert forward!=core
variants['forward']=forward
variants['forwardSwap']=forward.replace('std::move(b.begin(), b.end(), a.begin());','a.swap(b);')
variants['heapBuckets']=core.replace('std::array<std::size_t, 1 << B> cnt{};', 'std::vector<std::size_t> cnt(1 << B);').replace('cnt.fill(0);','std::fill(cnt.begin(), cnt.end(), 0);')
def invertedDigits(s):
    s=s.replace('return U(Descending ? U(maxV) - U(x) : U(x) - U(minV));','return U(U(x) - U(minV));')
    s=s.replace('''        auto pass = [&](auto first, auto output) {''','''        auto digit = [&](T x) {
            unsigned index = (key(x) >> shift) & mask;
            return Descending ? index ^ mask : index;
        };
        auto pass = [&](auto first, auto output) {''')
    s=s.replace('(key(first[j]) >> shift) & mask', 'digit(first[j])').replace('(key(first[0]) >> shift) & mask','digit(first[0])')
    return s
variants['invertedDigits']=invertedDigits(core)
variants['combined']=invertedDigits(variants['forwardSwap'])
variants['combined']=variants['combined'].replace('std::array<std::size_t, 1 << B> cnt{};', 'std::vector<std::size_t> cnt(1 << B);').replace('cnt.fill(0);','std::fill(cnt.begin(), cnt.end(), 0);')
for namespace,variant in variants.items():
    (build/(namespace+'.hpp')).write_text(base+operator.replace('namespace seq {','namespace '+namespace+' {')+variant+'}\n')
patch=(root/'tree_map_experiments/final_results/currentSortedVsSortKeys/RestoreInternalSort.patch').read_text().splitlines()
added=[line[1:] for line in patch if line.startswith('+') and not line.startswith('+++')]
legacy='\n'.join(added)
start=legacy.index('// Sorting is private')
end=legacy.index('        sortKeys(keys, compare);')
legacy=legacy[start:end].rstrip()
(build/'legacy.hpp').write_text(base+'namespace legacy {\n'+legacy+'\n}\n')
# Snapshot the exact tested sorting cores, keeping them separate from production code.
for path in build.glob('*.hpp'):
    (report/path.name).write_bytes(path.read_bytes())
(report/'sourceSha256.json').write_text(json.dumps({str(path.relative_to(root)):hashlib.sha256(path.read_bytes()).hexdigest() for path in [root/'list_helper_cpp17/ListHelperAdaptive.hpp',root/'tree_map_experiments/treeMap.hpp']},indent=2)+'\n')
print(build)
