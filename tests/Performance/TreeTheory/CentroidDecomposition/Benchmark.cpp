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
        benchmarkCheck(int(a.dfsOrder.size()) == n, "centroid traversal cardinality");
        std::vector<bool> seen(n);
        std::uint64_t checksum = 0;
        for (int x : a.dfsOrder) {
            benchmarkCheck(0 <= x and x < n and not seen[x], "centroid traversal permutation");
            seen[x] = true;
            checksum = benchmarkMix(checksum, x);
        }
        return checksum;
    });
}
