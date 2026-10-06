#include "../../../../src/GraphTheory/Dijkstra/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::vector<std::pair<int, int>>> g(n);
    for (int i = 1; i < n; ++i) {
        g[i - 1].push_back({i, int(1 + random() % 20)});
        if (input.shape)
            g[0].push_back({i, int(1 + random() % 100)});
    }
    return measure(input, "single-source shortest paths and cached queries; chain / shortcuts",
                   [&]() -> std::uint64_t {
                       Dijkstra<int> a(g);
                       std::uint64_t sum = 0;
                       for (int i = 0; i < n; ++i)
                           sum += a(0, i);
                       return sum;
                   });
}
