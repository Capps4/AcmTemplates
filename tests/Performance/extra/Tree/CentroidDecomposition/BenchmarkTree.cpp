#include "../../../../../src/extra/Tree/CentroidDecomposition/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    std::vector<std::vector<int>> g(in.n);
    for (int x = 1; x < in.n; ++x) {
        int p = in.shape ? 0 : x - 1;
        g[x].push_back(p);
        g[p].push_back(x);
    }
    return measure(in, "centroid tree output; chain / star", [&]() -> std::uint64_t {
        std::vector<std::vector<int>> cdt;
        CentroidDecomposition a(g, &cdt);
        benchmarkCheck(int(a.dfsOrder.size()) == in.n, "centroid traversal size");
        std::vector<int> pos(in.n);
        for (int i = 0; i < in.n; ++i)
            pos[a.dfsOrder[i]] = i;
        int cnt = 0;
        std::uint64_t sum = 0;
        for (int x = 0; x < in.n; ++x)
            for (int y : cdt[x]) {
                benchmarkCheck(pos[x] < pos[y], "parent before child");
                sum = benchmarkMix(sum, std::uint64_t(x) * in.n + y);
                ++cnt;
            }
        benchmarkCheck(cnt == in.n - 1, "centroid tree edge count");
        return sum;
    });
}
