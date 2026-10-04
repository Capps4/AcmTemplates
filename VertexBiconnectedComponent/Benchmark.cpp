#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
template<class T> std::uint64_t checksum(const T& result) {
    std::uint64_t sum = result.componentNum;
    for (std::size_t vertex = 0; vertex < result.csqt.size(); ++vertex)
        for (int next : result.csqt[vertex]) sum += (vertex + 1) * std::uint64_t(next + 1);
    return sum;
}
int main() {
    std::mt19937 rng(163);
    for (int shape = 0; shape < 3; ++shape) {
        int n = shape == 0 ? 10000 : 50000;
        std::vector<std::vector<int>> adj(n);
        auto add = [&](int x, int y) { adj[x].push_back(y); adj[y].push_back(x); };
        for (int x = 1; x < n; ++x) add(x, shape == 0 ? x - 1 : shape == 1 ? 0 : rng() % x);
        if (shape == 2) for (int x = 0; x < n; ++x) add(x, rng() % n);
        compare(shape == 0 ? "chain-10K" : shape == 1 ? "star-50K" : "random-multigraph-50K",
                [&] { return checksum(Legacy::VertexBC(adj)); }, [&] { return checksum(VertexBc(adj)); });
    }
}
