#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
int main() {
    Query::rangeScale(1000000);
    Legacy::Query::rangeScale(1000000);
    std::mt19937 rng(20261001);
    std::vector<Query> queries;
    std::vector<Legacy::Query> originals;
    for (int i = 0; i < 200000; ++i) {
        int l = rng() % 1000000, r = l + rng() % (1000001 - l);
        queries.emplace_back(l, r, i); originals.emplace_back(l, r, i);
    }
    auto work = [](auto a) {
        std::sort(a.begin(), a.end());
        std::uint64_t sum = 0;
        for (const auto& q : a) sum = sum * 1000003 + q.id;
        return sum;
    };
    compare("sort-200K", [&] { return work(originals); }, [&] { return work(queries); });
}
