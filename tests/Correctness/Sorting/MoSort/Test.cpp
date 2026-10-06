#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/Sorting/MoSort/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <algorithm>
#include <climits>
#include <tuple>
#include <vector>


int coreCases() {
    for (int n : {0, 1, 2, 100, 100000, INT_MAX}) {
        Query::rangeScale(n);
        int blockSize = int(std::sqrt(2.0 * n)) + 1;
        std::vector<Query> queries;
        for (int i = 0; i < 32; ++i) {
            int l = randomInt(0, n), r = randomInt(l, n);
            queries.emplace_back(l, r, i);
        }
        auto key = [&](const Query& q) {
            int block = q.l / blockSize;
            return std::pair{block, block % 2 == 0 ? q.r : -q.r};
        };
        std::sort(queries.begin(), queries.end());
        for (int i = 0; i < int(queries.size()); ++i) {
            CHECK(!(queries[i] < queries[i]));
            if (i) CHECK(key(queries[i - 1]) <= key(queries[i]));
        }
        for (int trial = 0; trial < 16; ++trial) {
        test_context::step = trial;
            const auto& a = queries[randomInt(0, 31)];
            const auto& b = queries[randomInt(0, 31)];
            CHECK((a < b) == (key(a) < key(b)));
        }
    }
    std::cout << "Snake-order oracle, strict weak order and INT_MAX scaling passed\n";
    return 0;
}

#include "../../../../src/Sorting/MoSort/code.hpp"
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
void verifyAdded(int n, int shape) {
    Query::rangeScale(n);
    std::vector<Query> q;
    std::vector<int> a(n);
    std::iota(a.begin(), a.end(), 1);
    for (int l = 0; l <= n; ++l)
        for (int r = l; r <= n; ++r)
            if (!shape or l == r or r == n)
                q.emplace_back(l, r, int(q.size()));
    int count = int(q.size());
    std::sort(q.begin(), q.end());
    std::vector<bool> seen(count);
    int l = 0, r = 0;
    long long sum = 0;
    for (auto x : q) {
        CHECK(!seen[x.id]);
        seen[x.id] = true;
        while (l > x.l)
            sum += a[--l];
        while (r < x.r)
            sum += a[r++];
        while (l < x.l)
            sum -= a[l++];
        while (r > x.r)
            sum -= a[--r];
        CHECK(sum == std::accumulate(a.begin() + x.l, a.begin() + x.r, 0LL));
    }
}

int run() {
    runCase("MoSort/empty", [] {
        verifyAdded(0, 0);
    });
    runCase("MoSort/single", [] {
        verifyAdded(1, 0);
    });
    runCase("MoSort/pair", [] {
        verifyAdded(2, 1);
    });
    runCase("MoSort/block-boundary", [] {
        verifyAdded(7, 0);
    });
    runCase("MoSort/mixed-window", [] {
        verifyAdded(17, 0);
    });
    return 0;
}
}

int main() {
    runCase("MoSort/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
