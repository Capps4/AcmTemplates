#include "../../../../../src/GraphTheory/Flow/MaxFlow/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <cstdint>
#include <tuple>
using Arc = std::tuple<int, int, long long>;
long long minCut(int n, const std::vector<Arc> &edges) {
    long long result = std::numeric_limits<long long>::max();
    for (int mask = 0; mask < (1 << n); ++mask) {
        if (!(mask & 1) || (mask >> (n - 1) & 1))
            continue;
        long long capacity = 0;
        for (auto [x, y, value] : edges)
            if ((mask >> x & 1) && !(mask >> y & 1))
                capacity += value;
        result = std::min(result, capacity);
    }
    return result;
}
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(int n, const std::vector<Arc> &es) {
    Flow<long long> f(n);
    for (auto [x, y, c] : es)
        f.add(x, y, c);
    auto want = minCut(n, es);
    auto a = f.work(0, n - 1, 1);
    CHECK(a == std::min(1LL, want));
    CHECK(a + f.work(0, n - 1) == want);
    CHECK(f.work(0, n - 1) == 0);
    auto seen = f.getReach(0);
    CHECK(seen[n - 1] == 't');
}

int main() {
    runCase("MaxFlow/01-no-path", [] {
        verifyAdded(3, {});
    });
    runCase("MaxFlow/02-zero-capacity", [] {
        verifyAdded(2, {{0, 1, 0LL}});
    });
    runCase("MaxFlow/03-single", [] {
        verifyAdded(2, {{0, 1, 7LL}});
    });
    runCase("MaxFlow/04-parallel", [] {
        verifyAdded(2, {{0, 1, 3LL}, {0, 1, 5LL}});
    });
    runCase("MaxFlow/05-bottleneck", [] {
        verifyAdded(3, {{0, 1, 99LL}, {1, 2, 2LL}});
    });
    runCase("MaxFlow/06-self-loop", [] {
        verifyAdded(2, {{0, 0, 8LL}, {0, 1, 4LL}});
    });
    runCase("MaxFlow/07-anti-parallel", [] {
        verifyAdded(3, {{0, 1, 7LL}, {1, 0, 3LL}, {1, 2, 5LL}});
    });
    runCase("MaxFlow/08-diamond", [] {
        verifyAdded(4, {{0, 1, 3LL}, {0, 2, 5LL}, {1, 3, 7LL}, {2, 3, 2LL}});
    });
    runCase("MaxFlow/09-reroute", [] {
        verifyAdded(4, {{0, 1, 1LL}, {0, 2, 1LL}, {1, 2, 1LL}, {1, 3, 1LL}, {2, 3, 1LL}});
    });
    runCase("MaxFlow/10-wide-capacity", [] {
        verifyAdded(2, {{0, 1, 4000000000000000000LL}});
    });
    return finishCases(10);
}
