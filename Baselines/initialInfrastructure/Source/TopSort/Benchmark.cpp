#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
std::uint64_t checksum(const std::vector<int>& order) {
    std::uint64_t sum = 0;
    for (std::size_t i = 0; i < order.size(); ++i) sum += std::uint64_t(order[i]) * (i + 1);
    return sum;
}
int main() {
    const int n = 200000;
    std::mt19937 rng(127);
    for (bool dense : {false, true}) {
        std::vector<std::vector<int>> graph(n);
        for (int x = 0; x + 1 < n; ++x) {
            graph[x].push_back(x + 1);
            if (dense) for (int edge = 0; edge < 5; ++edge) graph[x].push_back(x + 1 + rng() % (n - x - 1));
        }
        compare(dense ? "dag-degree6" : "chain", [&] { return checksum(Legacy::topSort(graph)); },
                [&] { return checksum(topSort(graph)); });
    }
}
