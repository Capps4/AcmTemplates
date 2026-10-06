#include "../../../../../src/GraphTheory/Flow/MaxFlow/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    (void)random;
    std::vector<std::tuple<int, int, int, int>> edges;
    for (int i = 1; i <= n; ++i) {
        edges.push_back({0, i, 1, 0});
        edges.push_back({i, n + 1, 1, input.shape ? i % 17 : 0});
    }

    return measure(input, "parallel two-edge augmenting paths; equal / varied costs",
                   [&]() -> std::uint64_t {
                       Flow<int> a(n + 2);
                       for (auto [x, y, c, w] : edges) {
                           (void)w;
                           a.add(x, y, c);
                       }
                       return a.work(0, n + 1);
                   });
}
