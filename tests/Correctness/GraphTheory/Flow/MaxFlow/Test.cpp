#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/GraphTheory/Flow/MaxFlow/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <cstdint>
#include <tuple>

using Arc = std::tuple<int, int, long long>;
long long minCut(int n, const std::vector<Arc>& edges) {
    long long result = std::numeric_limits<long long>::max();
    for (int mask = 0; mask < (1 << n); ++mask) {
        if (!(mask & 1) || (mask >> (n - 1) & 1)) continue;
        long long capacity = 0;
        for (auto [x, y, value] : edges) if ((mask >> x & 1) && !(mask >> y & 1)) capacity += value;
        result = std::min(result, capacity);
    }
    return result;
}
int coreCases() {
    for (int trial = 0; trial < 16; ++trial) {
        test_context::step = trial;
        int n = randomInt(2, 8);
        Flow<long long> flow(n);
        std::vector<Arc> edges;
        for (int i = 0, m = randomInt(0, 30); i < m; ++i) {
            int x = randomInt(0, n - 1), y = randomInt(0, n - 1);
            long long c = randomInt(0, 8), reverse = randomInt(0, 4);
            int id = flow.add(x, y, c, reverse);
            CHECK(id % 2 == 0 && flow.adj[id] == std::pair<int, long long>(y, c));
            edges.emplace_back(x, y, c); edges.emplace_back(y, x, reverse);
        }
        auto unchanged = flow.adj;
        CHECK(flow.work(0, 0) == 0 && flow.work(0, n - 1, 0) == 0 && flow.adj == unchanged);
        auto expected = minCut(n, edges);
        long long limit = randomInt(0, 30);
        auto first = flow.work(0, n - 1, limit);
        CHECK(first == std::min(expected, limit));
        CHECK(first + flow.work(0, n - 1) == expected && flow.work(0, n - 1) == 0);
        const auto& result = flow;
        auto reached = result.getReach(0);
        CHECK(reached[0] == 's' && reached[n - 1] == 't');
        long long capacity = 0;
        for (auto [x, y, value] : edges) if (reached[x] == 's' && reached[y] == 't') capacity += value;
        CHECK(capacity == expected);
        for (auto [x, y] : result.getCuts(0)) CHECK(reached[x] == 's' && reached[y] == 't');
        for (const auto& edge : flow.adj) CHECK(edge.second >= 0);
    }
    Flow<int> dynamic(0);
    CHECK(dynamic.newNode() == 0 && dynamic.newNode() == 1);
    dynamic.add(0, 1, 3, 5);
    CHECK(dynamic.work(0, 1) == 3 && dynamic.work(1, 0) == 8);
    CHECK(dynamic.newNode() == 2);
    dynamic.add(0, 2, 7);
    CHECK(dynamic.work(0, 2) == 7);
    using U = std::uint64_t;
    constexpr U max = std::numeric_limits<U>::max();
    Flow<U> full(2);
    full.add(0, 1, max); full.add(0, 1, max);
    CHECK(full.work(0, 1) == max && full.getReach(0)[1] == 's');
    CHECK(full.work(0, 1) == max && full.getReach(0)[1] == 't');
    Flow<double> fractions(4);
    fractions.add(0, 1, 0.5); fractions.add(0, 2, 0.75);
    fractions.add(1, 3, 0.5); fractions.add(2, 3, 0.75);
    CHECK(std::abs(fractions.work(0, 3) - 1.25) < 1e-12);
    const int n = 128;
    Flow<int> chain(n);
    for (int x = 1; x < n; ++x) chain.add(x - 1, x, 3);
    CHECK(chain.work(0, n - 1, 1) == 1 && chain.work(0, n - 1) == 2);
    CHECK(chain.work(0, n - 1) == 0 && chain.getReach(0)[n - 1] == 't');
    std::cout << "MaxFlow exhaustive cut oracle, bidirectional/parallel/self edges, limits/reuse, uint64 max, fractional, recursive 2K chain PASS\n";
    return 0;
}

#include "../../../../../src/GraphTheory/Flow/MaxFlow/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <cstdint>
#include <tuple>
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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

int run() {
    runCase("MaxFlow/no-path", [] {
        verifyAdded(3, {});
    });
    runCase("MaxFlow/zero-capacity", [] {
        verifyAdded(2, {{0, 1, 0LL}});
    });
    runCase("MaxFlow/single", [] {
        verifyAdded(2, {{0, 1, 7LL}});
    });
    runCase("MaxFlow/parallel", [] {
        verifyAdded(2, {{0, 1, 3LL}, {0, 1, 5LL}});
    });
    runCase("MaxFlow/bottleneck", [] {
        verifyAdded(3, {{0, 1, 99LL}, {1, 2, 2LL}});
    });
    runCase("MaxFlow/self-loop", [] {
        verifyAdded(2, {{0, 0, 8LL}, {0, 1, 4LL}});
    });
    runCase("MaxFlow/anti-parallel", [] {
        verifyAdded(3, {{0, 1, 7LL}, {1, 0, 3LL}, {1, 2, 5LL}});
    });
    runCase("MaxFlow/diamond", [] {
        verifyAdded(4, {{0, 1, 3LL}, {0, 2, 5LL}, {1, 3, 7LL}, {2, 3, 2LL}});
    });
    runCase("MaxFlow/reroute", [] {
        verifyAdded(4, {{0, 1, 1LL}, {0, 2, 1LL}, {1, 2, 1LL}, {1, 3, 1LL}, {2, 3, 1LL}});
    });
    runCase("MaxFlow/wide-capacity", [] {
        verifyAdded(2, {{0, 1, 4000000000000000000LL}});
    });
    return 0;
}
}

int main() {
    runCase("MaxFlow/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
