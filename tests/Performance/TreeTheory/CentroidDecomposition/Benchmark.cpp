#include "../../../../src/TreeTheory/CentroidDecomposition/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::vector<int>> g(n);
    for (int i = 1; i < n; ++i) {
        int p = input.shape ? 0 : i - 1;
        g[p].push_back(i);
        g[i].push_back(p);
    }
    (void)random;
    return measure(input, "chain / star; build and traversal", [&]() -> std::uint64_t {
        CentroidDecomposition a(g);
        return std::accumulate(a.dfsOrder.begin(), a.dfsOrder.end(), std::uint64_t(0));
    });
}
