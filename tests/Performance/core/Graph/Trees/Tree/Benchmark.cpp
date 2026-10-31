#include "../../../../../../src/core/Graph/Trees/Tree/code.hpp"
#include "../../../../../Support/BenchmarkSupport.hpp"
#include "../../../../../Support/TestStack.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::vector<std::pair<int, int>>> g(n);
    for (int i = 1; i < n; ++i) {
        int p = input.shape ? 0 : i - 1;
        g[p].emplace_back(i, 1000000000);
        g[i].emplace_back(p, 1000000000);
    }
    (void)random;
    return withTestStack([&] {
        CHECK(measure(input, "weighted chain / star; build and distances", [&]() -> std::uint64_t {
            auto copy = g;
            FullTree<std::pair<int, int>> a(copy);
            std::uint64_t sum = 0;
            for (int i = 0; i < n; ++i) {
                int dis = input.shape ? (i == n - 1 ? 0 : (i == 0 ? 1 : 2)) : n - 1 - i;
                benchmarkCheck(a.dist(i, n - 1) == dis, "edge count");
                benchmarkCheck(a.wtDist(i, n - 1) == 1000000000LL * dis, "weighted distance");
                sum += a.wtDist(i, n - 1);
            }
            return sum;
        }) == 0);
    }, false);
}
