#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
struct Record { int key; std::array<int, 64> payload{}; };
int main() {
    std::mt19937 rng(20261001);
    auto sum = [](const auto& a) { return std::accumulate(a.begin(), a.end(), std::uint64_t{}); };
    for (int range : {100, 1000000}) {
        std::vector<int> a(300000);
        for (int& x : a) x = rng() % range;
        std::string name = "integers-range-" + std::to_string(range);
        compare(name.c_str(), [&] { return sum(Legacy::countingSortOrder(a)); },
                              [&] { return sum(countingSortOrder(a)); });
    }
    std::vector<Record> records(100000);
    for (auto& record : records) record.key = rng() % 10000;
    auto key = [](const Record& record) { return record.key; };
    compare("256-byte-records", [&] { return sum(Legacy::countingSortOrder(records, key)); },
                                 [&] { return sum(countingSortOrder(records, key)); });
}
