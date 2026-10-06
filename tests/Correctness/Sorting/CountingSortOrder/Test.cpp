#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/Sorting/CountingSortOrder/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <array>
#include <numeric>
#include <memory>


struct Record { int key; std::array<int, 64> payload; };
int coreCases() {
    CHECK(countingSortOrder(std::vector<int>{}).empty());
    for (int trial = 0; trial < 16; ++trial) {
        test_context::step = trial;
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
    return 0;
}

#include "../../../../src/Sorting/CountingSortOrder/code.hpp"
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
void verifyAdded(const std::vector<int> &a) {
    test_context::describe(a);
    std::vector<int> e(a.size());
    std::iota(e.begin(), e.end(), 0);
    std::stable_sort(e.begin(), e.end(), [&](int x, int y) {
        return a[x] < a[y];
    });
    CHECK(countingSortOrder(a) == e);
    std::vector<std::pair<int, int>> b;
    for (int i = 0; i < int(a.size()); ++i)
        b.emplace_back(a[i], i);
    CHECK(countingSortOrder(b, [](auto x) {
              return x.first;
          }) == e);
}

int run() {
    runCase("CountingSortOrder/empty", [] {
        verifyAdded({});
    });
    runCase("CountingSortOrder/single", [] {
        verifyAdded({0});
    });
    runCase("CountingSortOrder/duplicates", [] {
        verifyAdded({3, 3, 3});
    });
    runCase("CountingSortOrder/descending", [] {
        verifyAdded({4, 3, 2, 1, 0});
    });
    runCase("CountingSortOrder/wide-gap", [] {
        verifyAdded({0, 1000});
    });
    runCase("CountingSortOrder/interleaved", [] {
        verifyAdded({7, 4, 7, 2, 4, 0});
    });
    return 0;
}
}

int main() {
    runCase("CountingSortOrder/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
