#include "../../../../../src/DataStructures/BaseDataStructures/DisjointSetUnion/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::pair<int, int>> a;
    for (int i = 0; i < n; ++i)
        a.emplace_back(i, input.shape ? 0 : int(random() % n));
    return measure(input, "DSU union/find; random / shared root", [&]() -> std::uint64_t {
        DSU d(n);
        for (auto [x, y] : a)
            d.Union(x, y);
        std::uint64_t sum = 0;
        for (int i = 0; i < n; ++i)
            sum += d.size[d.find(i)];
        return sum;
    });
}
