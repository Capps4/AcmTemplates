#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
using Arc = std::tuple<int, int, long long>;
template<class T> std::uint64_t solve(int n, const std::vector<Arc>& edges) {
    T flow(n);
    for (auto [x, y, capacity] : edges) flow.add(x, y, capacity);
    return flow.work(0, n - 1);
}
int main() {
    std::mt19937 rng(173);
    for (int shape = 0; shape < 3; ++shape) {
        int n = shape == 0 ? 10000 : shape == 1 ? 50000 : 6002;
        std::vector<Arc> edges;
        if (shape == 0) for (int x = 1; x < n; ++x) edges.emplace_back(x - 1, x, 7);
        if (shape == 1) for (int x = 1; x + 1 < n; ++x) {
            edges.emplace_back(0, x, 7); edges.emplace_back(x, n - 1, 7);
        }
        if (shape == 2) {
            constexpr int width = 100, layers = 60;
            for (int x = 0; x < width; ++x) edges.emplace_back(0, 1 + x, 10);
            for (int layer = 0; layer + 1 < layers; ++layer) for (int x = 0; x < width; ++x)
                for (int i = 0; i < 3; ++i) edges.emplace_back(1 + layer * width + x,
                    1 + (layer + 1) * width + rng() % width, rng() % 7 + 1);
            for (int x = 0; x < width; ++x) edges.emplace_back(1 + (layers - 1) * width + x, n - 1, 10);
        }
        compare(shape == 0 ? "chain-10K" : shape == 1 ? "wide-depth2-50K" : "layered-60x100",
                [&] { return solve<Legacy::Flow<long long>>(n, edges); },
                [&] { return solve<Flow<long long>>(n, edges); });
    }
}
