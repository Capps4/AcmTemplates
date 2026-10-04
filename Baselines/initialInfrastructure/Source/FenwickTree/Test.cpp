#include "Final.hpp"
#include "../TestSupport.hpp"
#include <algorithm>
#include <climits>
#include <memory>
#include <numeric>

int main() {
    Fenwick<long long> empty(0);
    CHECK(empty.posQuery(0 - 1) == 0 && empty.select(0) == -1);
    for (int trial = 0; trial < 100; ++trial) {
        int n = randomInt(1, 100);
        std::vector<long long> a(n);
        for (auto& value : a) value = randomInt(0, 10);
        Fenwick sum(a);
        Fenwick<long long> incremental(n);
        for (int i = 0; i < n; ++i) incremental.modify(i, a[i]);
        for (int step = 0; step < 300; ++step) {
            int i = randomInt(0, n - 1), delta = randomInt(0, 10);
            a[i] += delta; sum.modify(i, delta); incremental.modify(i, delta);
            int l = randomInt(0, n), r = randomInt(l, n);
            CHECK(sum.rangeQuery(l, r) == std::accumulate(a.begin() + l, a.begin() + r, 0LL));
            long long prefix = std::accumulate(a.begin(), a.begin() + r, 0LL);
            CHECK(sum.posQuery(r - 1) == prefix && sum.posQuery(r - 1) == prefix);
            CHECK(incremental.posQuery(r - 1) == prefix);
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
            CHECK(xorTree.posQuery(r - 1) == x && maxTree.posQuery(r - 1) == maximum);
        }
    }
    Fenwick orTree(std::vector<bool>{false, true, false}, std::logical_or<bool>{}, false);
    CHECK(!orTree.posQuery(1 - 1) && orTree.posQuery(2 - 1));
    std::cout << "Random sum/update/select oracles, O(n) build, explicit max identity, move-only XOR and vector<bool> passed\n";
}
