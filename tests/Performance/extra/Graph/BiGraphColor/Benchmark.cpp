#include "../../../../../src/extra/Graph/BiGraphColor/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::vector<std::vector<int>> g(in.n);
    for (int x = 1; x < in.n; ++x) {
        g[x].push_back(x - 1);
        g[x - 1].push_back(x);
    }
    if (in.shape) {
        int x = in.n >= 3 ? in.n - 3 : in.n - 1;
        g[x].push_back(in.n - 1);
        g[in.n - 1].push_back(x);
    }
    return measure(in, "bipartite chain / odd cycle after long prefix", [&]() -> std::uint64_t {
        BiGraphColor a(g);
        benchmarkCheck(a.isBiGraph == (in.shape == 0), "known bipartiteness");
        std::uint64_t sum = 0;
        for (int x = 0; x < in.n; ++x) {
            if (in.shape == 0) {
                benchmarkCheck(a.col[x] == x % 2, "chain coloring");
            }
            sum = benchmarkMix(sum, a.col[x] + 1);
        }
        return sum;
    });
}
