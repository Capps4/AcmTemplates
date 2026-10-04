#include <bits/stdc++.h>
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
struct Arc { int x, y, cap, fee; };
template <template <class, class> class Solver>
std::uint64_t solve(int n, const std::vector<Arc>& arcs) {
    Solver<int, long long> solver(n);
    for (auto [x, y, cap, fee] : arcs) solver.add(x, y, cap, fee);
    auto [flow, cost] = solver.work(0, n - 1);
    return std::uint64_t(flow) * 1000000007ULL + std::uint64_t(cost);
}
void scenario(const char* name, int n, const std::vector<Arc>& arcs) {
    compare(name, [&] { return solve<Legacy::CostFlow>(n, arcs); }, [&] { return solve<CostFlow>(n, arcs); });
}
int main() {
    std::mt19937 rng(20261001);
    std::vector<Arc> chain;
    for (int x = 1; x < 50000; ++x) chain.push_back({x - 1, x, 100, 1});
    scenario("chain-50K", 50000, chain);
    for (int size : {60, 150}) {
        std::vector<Arc> arcs;
        for (int x = 0; x < size; ++x) {
            arcs.push_back({0, 1 + x, 1, 0});
            arcs.push_back({1 + size + x, 1 + 2 * size, 1, 0});
            for (int y = 0; y < size; ++y) arcs.push_back({1 + x, 1 + size + y, 1, int(rng() % 1000)});
        }
        scenario(size == 60 ? "assignment-60" : "assignment-150", 2 * size + 2, arcs);
    }
    std::vector<Arc> dag;
    for (int x = 1; x < 2000; ++x) dag.push_back({x - 1, x, 10, -1});
    for (int i = 0; i < 10000; ++i) {
        int x = rng() % 1999, y = x + 1 + rng() % (1999 - x);
        dag.push_back({x, y, 1 + int(rng() % 10), -int(rng() % 5)});
    }
    scenario("negative-dag-2K", 2000, dag);
}
