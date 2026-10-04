#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
template<class T> std::uint64_t checksum(const T& tree) {
    std::uint64_t sum = 0;
    for (std::size_t i = 0; i < tree.dfsOrder.size(); ++i) sum += std::uint64_t(tree.dfsOrder[i]) * (i + 1);
    return sum;
}
int main() {
    std::mt19937 rng(151);
    for (int shape = 0; shape < 3; ++shape) {
        int n = shape == 0 ? 10000 : 100000;
        std::vector<std::vector<int>> adj(n);
        for (int x = 1; x < n; ++x) {
            int parent = shape == 0 ? x - 1 : shape == 1 ? 0 : rng() % x;
            adj[x].push_back(parent); adj[parent].push_back(x);
        }
        compare(shape == 0 ? "chain-10K" : shape == 1 ? "star-100K" : "random-tree-100K",
                [&] { return checksum(Legacy::CentroidDecomposition(adj)); },
                [&] { return checksum(CentroidDecomposition(adj)); });
    }
}
