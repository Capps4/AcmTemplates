#include "../../../../src/Sorting/CountingSortOrder/code.hpp"
#include "../../../Support/CaseSupport.hpp"
void verifyAdded(const std::vector<int> &a) {
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

int main() {
    runCase("CountingSortOrder/01-order-00", [] {
        verifyAdded({});
    });
    runCase("CountingSortOrder/02-order-01", [] {
        verifyAdded({0});
    });
    runCase("CountingSortOrder/03-order-02", [] {
        verifyAdded({3, 3, 3});
    });
    runCase("CountingSortOrder/04-order-03", [] {
        verifyAdded({0, 1, 2, 3});
    });
    runCase("CountingSortOrder/05-order-04", [] {
        verifyAdded({4, 3, 2, 1, 0});
    });
    runCase("CountingSortOrder/06-order-05", [] {
        verifyAdded({2, 0, 2, 1, 0});
    });
    runCase("CountingSortOrder/07-order-06", [] {
        verifyAdded({0, 1000});
    });
    runCase("CountingSortOrder/08-order-07", [] {
        verifyAdded({9, 0, 9, 0, 9});
    });
    runCase("CountingSortOrder/09-order-08", [] {
        verifyAdded({1, 0, 1, 0});
    });
    runCase("CountingSortOrder/10-order-09", [] {
        verifyAdded({7, 4, 7, 2, 4, 0});
    });
    return finishCases(10);
}
