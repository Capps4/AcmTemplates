#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
struct CapturedCmp {
    std::vector<int> thresholds = std::vector<int>(256, 0);
    bool operator()(int a, int b) const { return a + thresholds[0] < b + thresholds[0]; }
};
int main() {
    std::mt19937 rng(20261001);
    std::vector<int> values(100000);
    for (int& x : values) x = rng() % 1000000;
    compare("build-100K", [&] { Legacy::RMQ<int> tree(values); return std::uint64_t(tree(0, values.size())); },
                          [&] { RMQ tree(values); return std::uint64_t(tree(0, values.size())); });
    std::vector<std::pair<int, int>> queries(300000);
    for (auto& [l, r] : queries) { l = rng() % values.size(); r = l + 1 + rng() % (values.size() - l); }
    Legacy::RMQ<int> oldTree(values);
    RMQ tree(values);
    compare("query-300K", [&] { std::uint64_t sum = 0; for (auto [l, r] : queries) sum += oldTree(l, r); return sum; },
                          [&] { std::uint64_t sum = 0; for (auto [l, r] : queries) sum += tree(l, r); return sum; });
    values.resize(30000);
    compare("build-stateful-comparator-30K", [&] {
        Legacy::RMQ<int, CapturedCmp> table(values); return std::uint64_t(table(0, values.size()));
    }, [&] {
        RMQ table(values, CapturedCmp{}); return std::uint64_t(table(0, values.size()));
    });
}
