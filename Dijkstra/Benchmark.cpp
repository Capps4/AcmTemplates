#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
int main() {
    std::mt19937 rng(20261001);
    for (auto [n, degree] : {std::pair{5000, 6}, std::pair{1000, 30}}) {
        std::vector<std::vector<std::pair<int, int>>> graph(n);
        for (int x = 0; x < n; ++x) {
            graph[x].emplace_back((x + 1) % n, 1);
            for (int i = 1; i < degree; ++i) graph[x].emplace_back(rng() % n, rng() % 100);
        }
        std::string label = "20-sources-n" + std::to_string(n) + "-degree" + std::to_string(degree);
        compare(label.c_str(), [&] {
            Legacy::Dijkstra<int, long long> solver(graph);
            std::uint64_t sum = 0;
            for (int source = 0; source < 20; ++source) for (int y = 0; y < n; ++y) sum += solver(source, y);
            return sum;
        }, [&] {
            Dijkstra<int> solver(graph);
            std::uint64_t sum = 0;
            for (int source = 0; source < 20; ++source) for (int y = 0; y < n; ++y) sum += solver(source, y);
            return sum;
        });
    }
}
