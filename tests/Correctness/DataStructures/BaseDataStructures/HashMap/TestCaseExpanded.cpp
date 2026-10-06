#include "../../../../../src/DataStructures/BaseDataStructures/HashMap/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(long long seed, int n) {
    _hashmap::Impl<long long, 7, 512> h;
    std::map<long long, long long> e;
    for (int i = 0; i < n; ++i) {
        long long k = (i % 13 - 6) * 7 + seed;
        h[k] += i - 5;
        e[k] += i - 5;
    }
    for (auto [k, v] : e) {
        CHECK(h(k) == v);
    }
    CHECK(h(LLONG_MAX) == 0);
    CHECK(h(LLONG_MIN) == 0);
    std::map<long long, long long> got;
    for (auto [k, v] : h)
        got[k] = v;
    CHECK(got == e);
    auto moved = std::move(h);
    for (auto [k, v] : e)
        CHECK(moved(k) == v);
    moved.clear();
    CHECK(moved(0) == 0);
    moved[seed] = 9;
    CHECK(moved(seed) == 9);
}

int main() {
    runCase("HashMap/01-hash-00", [] {
        verifyAdded(0, 0);
    });
    runCase("HashMap/02-hash-01", [] {
        verifyAdded(1, 1);
    });
    runCase("HashMap/03-hash-02", [] {
        verifyAdded(7, 2);
    });
    runCase("HashMap/04-hash-03", [] {
        verifyAdded(11, 3);
    });
    runCase("HashMap/05-hash-04", [] {
        verifyAdded(13, 10);
    });
    runCase("HashMap/06-hash-05", [] {
        verifyAdded(17, 15);
    });
    runCase("HashMap/07-hash-06", [] {
        verifyAdded(23, 20);
    });
    runCase("HashMap/08-hash-07", [] {
        verifyAdded(31, 40);
    });
    runCase("HashMap/09-hash-08", [] {
        verifyAdded(47, 80);
    });
    runCase("HashMap/10-hash-09", [] {
        verifyAdded(97, 120);
    });
    return finishCases(10);
}
