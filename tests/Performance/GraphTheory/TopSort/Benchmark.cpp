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
        benchmarkCheck(int(a.size()) == n, "DAG visits every vertex");
        std::vector<int> rank(n, -1);
        std::uint64_t checksum = 0;
        for (int i = 0; i < n; ++i) {
            benchmarkCheck(0 <= a[i] and a[i] < n and rank[a[i]] == -1, "unique vertex order");
            rank[a[i]] = i;
            checksum = benchmarkMix(checksum, a[i]);
        }
        for (int x = 0; x < n; ++x)
            for (int y : g[x]) benchmarkCheck(rank[x] < rank[y], "edge direction in order");
        return checksum;
    });
}
