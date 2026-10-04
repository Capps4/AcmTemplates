#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
template<class T> std::uint64_t checksum(const T& result) {
    std::uint64_t sum = result.cntBlock + result.componentNum;
    for (std::size_t i = 0; i < result.bel.size(); ++i) sum += std::uint64_t(result.bel[i]) * (i + 1);
    for (const auto& row : result.g) sum += row.size();
    return sum;
}
int main() {
    std::mt19937 rng(157);
    for (int shape = 0; shape < 3; ++shape) {
        int n = shape == 0 ? 10000 : 50000;
        std::vector<std::vector<int>> adj(n);
        auto add = [&](int x, int y) { adj[x].push_back(y); adj[y].push_back(x); };
        for (int x = 1; x < n; ++x) add(x, shape == 0 ? x - 1 : shape == 1 ? 0 : rng() % x);
        if (shape == 2) for (int x = 0; x < n; ++x) add(x, rng() % n);
        compare(shape == 0 ? "chain-10K" : shape == 1 ? "star-50K" : "random-multigraph-50K",
                [&] { return checksum(Legacy::EdgeBC(adj)); }, [&] { return checksum(EdgeBc(adj)); });
    }
}
