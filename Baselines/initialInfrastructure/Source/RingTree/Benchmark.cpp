#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
template<class T> std::uint64_t checksum(const T& result) {
    std::uint64_t sum = 0;
    for (std::size_t vertex = 0; vertex < result.g.size(); ++vertex)
        for (int child : result.g[vertex]) sum += (vertex + 1) * std::uint64_t(child + 1);
    for (const auto& ring : result.rings) for (std::size_t i = 0; i < ring.size(); ++i)
        sum += std::uint64_t(ring[i] + 1) * (i + 1);
    return sum;
}
int main() {
    const int n = 200000;
    std::mt19937 rng(167);
    for (bool random : {false, true}) {
        std::vector<int> link(n);
        for (int x = 0; x < n; ++x) link[x] = random ? rng() % n : std::min(x + 1, n - 1);
        compare(random ? "functional-random-200K" : "functional-chain-200K",
                [&] { return checksum(Legacy::RingTree(link)); }, [&] { return checksum(RingTree(link)); });
    }
    std::vector<std::vector<int>> adj(n);
    for (int x = 0; x < n; ++x) {
        int next = x + 1 < n ? x + 1 : n - 3;
        adj[x].push_back(next); adj[next].push_back(x);
    }
    compare("undirected-chain-triangle-200K", [&] { return checksum(Legacy::RingTree(adj)); },
            [&] { return checksum(RingTree(adj)); });
}
