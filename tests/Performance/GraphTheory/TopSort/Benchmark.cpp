#include "../../../../src/GraphTheory/TopSort/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::vector<int>> g(n);
    for (int i = 1; i < n; ++i) {
        int p = input.shape ? 0 : i - 1;
        g[p].push_back(i);
    }
    (void)random;
    return measure(input, "chain / star; build and traversal", [&]() -> std::uint64_t {
        auto a = topSort(g);
        return std::accumulate(a.begin(), a.end(), std::uint64_t(0));
    });
}
