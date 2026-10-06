#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/DataStructures/TreeDataStructures/FenwickTree/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <algorithm>
#include <climits>
#include <memory>
#include <numeric>


int coreCases() {
    Fenwick<long long> empty(0);
    CHECK(empty.query(0 - 1) == 0 && empty.select(0) == -1);
    for (int trial = 0; trial < 8; ++trial) {
        test_context::step = trial;
        int n = randomInt(1, 100);
        std::vector<long long> a(n);
        for (auto& value : a) value = randomInt(0, 10);
        Fenwick sum(a);
        Fenwick<long long> incremental(n);
        for (int i = 0; i < n; ++i) incremental.modify(i, a[i]);
        for (int step = 0; step < 32; ++step) {
        test_context::step = step;
            int i = randomInt(0, n - 1), delta = randomInt(0, 10);
            a[i] += delta; sum.modify(i, delta); incremental.modify(i, delta);
            int l = randomInt(0, n), r = randomInt(l, n);
            CHECK((sum.query(r - 1) - sum.query(l - 1)) == std::accumulate(a.begin() + l, a.begin() + r, 0LL));
            long long prefix = std::accumulate(a.begin(), a.begin() + r, 0LL);
            CHECK(sum.query(r - 1) == prefix && sum.query(r - 1) == prefix);
            CHECK(incremental.query(r - 1) == prefix);
            long long limit = randomInt(-1, int(std::accumulate(a.begin(), a.end(), 0LL)) + 10);
            int length = 0; long long total = 0;
            while (length < n && total + a[length] <= limit) total += a[length++];
            CHECK(sum.select(limit) == length - 1);
        }
        auto merge = [offset = std::make_unique<int>(0)](int x, int y) { return (x ^ y) + *offset; };
        std::vector<int> values(n);
        for (int& x : values) x = randomInt(-100, 100);
        Fenwick xorTree(values, std::move(merge), 0);
        Fenwick<int, Max<int>> maxTree(values, Max<int>{}, INT_MIN);
        for (int r = 0; r <= n; ++r) {
            int x = 0, maximum = INT_MIN;
            for (int i = 0; i < r; ++i) { x ^= values[i]; maximum = std::max(maximum, values[i]); }
            CHECK(xorTree.query(r - 1) == x && maxTree.query(r - 1) == maximum);
        }
    }
    Fenwick orTree(std::vector<bool>{false, true, false}, std::logical_or<bool>{}, false);
    CHECK(!orTree.query(1 - 1) && orTree.query(2 - 1));
    std::cout << "Random sum/update/select oracles, O(n) build, explicit max identity, move-only XOR and vector<bool> passed\n";
    return 0;
}

#include "../../../../../src/DataStructures/TreeDataStructures/FenwickTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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

int run() {
    runCase("FenwickTree/empty", [] {
        verifyAdded({});
    });
    runCase("FenwickTree/single", [] {
        verifyAdded({0});
    });
    runCase("FenwickTree/duplicates", [] {
        verifyAdded({3, 3, 3});
    });
    runCase("FenwickTree/descending", [] {
        verifyAdded({4, 3, 2, 1, 0});
    });
    runCase("FenwickTree/wide-gap", [] {
        verifyAdded({0, 1000});
    });
    runCase("FenwickTree/interleaved", [] {
        verifyAdded({7, 4, 7, 2, 4, 0});
    });
    return 0;
}
}

int main() {
    runCase("FenwickTree/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
