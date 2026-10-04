#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
template<class Tree> std::uint64_t build(const std::vector<std::vector<int>>& source) {
    auto graph = source; Tree tree(graph); std::uint64_t sum = 0;
    for (int x = 0; x < tree.n; ++x) sum += std::uint64_t(tree.dfn[x] + 1) * (x + 1);
    return sum;
}
int main() {
    std::mt19937 rng(181);
    for (int shape = 0; shape < 3; ++shape) {
        int n = shape == 0 ? 10000 : 100000;
        std::vector<std::vector<int>> graph(n);
        for (int x = 1; x < n; ++x) { int y = shape == 0 ? x - 1 : shape == 1 ? 0 : rng() % x; graph[x].push_back(y); graph[y].push_back(x); }
        compare(shape == 0 ? "chain-10K" : shape == 1 ? "star-100K" : "random-100K",
            [&] { return build<Legacy::FullTree<int>>(graph); }, [&] { return build<FullTree<int>>(graph); });
    }
    const int n = 100000;
    std::vector<std::vector<int>> oldGraph(n);
    for (int x = 1; x < n; ++x) { int y = rng() % x; oldGraph[x].push_back(y); oldGraph[y].push_back(x); }
    auto graph = oldGraph; Legacy::FullTree<int> old(oldGraph); FullTree<int> current(graph);
    std::vector<std::pair<int, int>> queries(100000); for (auto& [x, y] : queries) { x = rng() % n; y = rng() % n; }
    compare("lca-100K", [&] { std::uint64_t sum = 0; for (auto [x,y] : queries) sum += old.lca(x,y); return sum; },
                         [&] { std::uint64_t sum = 0; for (auto [x,y] : queries) sum += current.lca(x,y); return sum; });
}
