#include "Final.hpp"
#include "../TestSupport.hpp"
#include <array>
#include <numeric>
#include <memory>

struct Record { int key; std::array<int, 64> payload; };
int main() {
    CHECK(countingSortOrder(std::vector<int>{}).empty());
    for (int trial = 0; trial < 2000; ++trial) {
        std::vector<int> a(randomInt(0, 300));
        for (int& x : a) x = randomInt(0, 500);
        std::vector<int> expected(a.size());
        std::iota(expected.begin(), expected.end(), 0);
        std::stable_sort(expected.begin(), expected.end(), [&](int i, int j) { return a[i] < a[j]; });
        CHECK(countingSortOrder(a) == expected);
        std::vector<Record> records(a.size());
        for (int i = 0; i < int(a.size()); ++i) records[i].key = a[i];
        auto key = [offset = std::make_unique<int>(7)](const Record& record) { return record.key + *offset; };
        CHECK(countingSortOrder(records, std::move(key)) == expected);
    }
    CHECK(countingSortOrder(std::vector<int>{1000000, 0, 1000000}) == std::vector<int>({1, 0, 2}));
    std::cout << "Stable-sort oracle, empty/sparse keys, large records and move-only key passed\n";
}
