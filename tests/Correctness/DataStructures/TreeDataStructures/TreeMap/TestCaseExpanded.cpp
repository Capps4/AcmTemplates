#include "../../../../../src/DataStructures/TreeDataStructures/TreeMap/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(const std::vector<int> &a) {
    TreeMap<int, int> d;
    std::map<int, int> e;
    auto verify = [&] {
        CHECK(d.size() == int(e.size()));
        int k = 0;
        for (auto [x, v] : e) {
            CHECK(d.contains(x));
            CHECK(d(x) == v);
            CHECK(d.keyAt(k) == x);
            CHECK(d.rankOf(x) == k);
            ++k;
        }
        for (int x = -10; x < 10; ++x)
            CHECK(d.rankOf(x) == std::distance(e.begin(), e.lower_bound(x)));
    };
    for (int x : a) {
        d.insertOrAssign(x, x + 10);
        e[x] = x + 10;
        verify();
    }
    auto saved = d;
    for (int x : a) {
        CHECK(d.erase(x) == bool(e.erase(x)));
        verify();
    }
    CHECK(saved.size() >= d.size());
    d.clear();
    CHECK(d.empty());
    d[19] = 7;
    CHECK(d(19) == 7);
}

int main() {
    runCase("TreeMap/01-ordered-00", [] {
        verifyAdded({});
    });
    runCase("TreeMap/02-ordered-01", [] {
        verifyAdded({-4});
    });
    runCase("TreeMap/03-ordered-02", [] {
        verifyAdded({-1, -1, -1});
    });
    runCase("TreeMap/04-ordered-03", [] {
        verifyAdded({-4, -3, -2, -1});
    });
    runCase("TreeMap/05-ordered-04", [] {
        verifyAdded({0, -1, -2, -3, -4});
    });
    runCase("TreeMap/06-ordered-05", [] {
        verifyAdded({-2, -4, -2, -3, -4});
    });
    runCase("TreeMap/07-ordered-06", [] {
        verifyAdded({-4, 996});
    });
    runCase("TreeMap/08-ordered-07", [] {
        verifyAdded({5, -4, 5, -4, 5});
    });
    runCase("TreeMap/09-ordered-08", [] {
        verifyAdded({-3, -4, -3, -4});
    });
    runCase("TreeMap/10-ordered-09", [] {
        verifyAdded({3, 0, 3, -2, 0, -4});
    });
    return finishCases(10);
}
