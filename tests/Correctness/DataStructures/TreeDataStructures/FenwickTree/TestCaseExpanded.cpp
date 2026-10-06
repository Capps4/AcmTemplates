#include "../../../../../src/DataStructures/TreeDataStructures/FenwickTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(std::vector<int> a) {
    Fenwick<int> d(a);
    CHECK(d.query(-1) == 0);
    auto verifyState = [&] {
        int total = std::accumulate(a.begin(), a.end(), 0);
        for (int i = 0; i < int(a.size()); ++i)
            CHECK(d.query(i) == std::accumulate(a.begin(), a.begin() + i + 1, 0));
        for (int k = 0; k <= total + 1; ++k) {
            int e = -1, sum = 0;
            for (int i = 0; i < int(a.size()); ++i) {
                sum += a[i];
                if (sum <= k)
                    e = i;
            }
            CHECK(d.select(k) == e);
        }
    };
    verifyState();
    for (int i = 0; i < int(a.size()); ++i) {
        d.modify(i, i + 1);
        a[i] += i + 1;
        verifyState();
    }
}

int main() {
    runCase("FenwickTree/01-prefix-00", [] {
        verifyAdded({});
    });
    runCase("FenwickTree/02-prefix-01", [] {
        verifyAdded({0});
    });
    runCase("FenwickTree/03-prefix-02", [] {
        verifyAdded({3, 3, 3});
    });
    runCase("FenwickTree/04-prefix-03", [] {
        verifyAdded({0, 1, 2, 3});
    });
    runCase("FenwickTree/05-prefix-04", [] {
        verifyAdded({4, 3, 2, 1, 0});
    });
    runCase("FenwickTree/06-prefix-05", [] {
        verifyAdded({2, 0, 2, 1, 0});
    });
    runCase("FenwickTree/07-prefix-06", [] {
        verifyAdded({0, 1000});
    });
    runCase("FenwickTree/08-prefix-07", [] {
        verifyAdded({9, 0, 9, 0, 9});
    });
    runCase("FenwickTree/09-prefix-08", [] {
        verifyAdded({1, 0, 1, 0});
    });
    runCase("FenwickTree/10-prefix-09", [] {
        verifyAdded({7, 4, 7, 2, 4, 0});
    });
    return finishCases(10);
}
