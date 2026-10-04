#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
int main() {
    constexpr int n = 200000;
    std::vector<long long> values(n, 1);
    std::mt19937 rng(20261001);
    std::vector<int> indices(n), limits(n);
    for (int i = 0; i < n; ++i) { indices[i] = rng() % n; limits[i] = rng() % n; }
    compare("linear-build-200K-x20", [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20; ++i) { Legacy::Fenwick<long long> tree(values); sum += tree.posQuery(n - 1); }
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20; ++i) { Fenwick tree(values); sum += tree.posQuery(n - 1); }
        return sum;
    });
    auto operations = [&](auto& tree) {
        std::uint64_t sum = 0;
        for (int i : indices) { tree.modify(i, 1); sum += tree.posQuery(i); }
        return sum;
    };
    compare("modify-query-200K", [&] { Legacy::Fenwick<long long> tree(values); return operations(tree); },
                                  [&] { Fenwick tree(values); return operations(tree); });
    Legacy::Fenwick<long long> before(values);
    Fenwick after(values);
    compare("select-200K", [&] { std::uint64_t sum = 0; for (int k : limits) sum += before.select(k); return sum; },
                           [&] { std::uint64_t sum = 0; for (int k : limits) sum += after.select(k); return sum; });
}
