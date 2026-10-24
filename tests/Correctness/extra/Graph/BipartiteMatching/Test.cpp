#include "../../../../../src/extra/Graph/BipartiteMatching/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int oracle(const std::vector<std::vector<int>> &g, int x, unsigned mask) {
    if (x == int(g.size()))
        return 0;
    int best = oracle(g, x + 1, mask);
    for (int y : g[x])
        if (!(mask >> y & 1))
            best = std::max(best, 1 + oracle(g, x + 1, mask | (1U << y)));
    return best;
}

void checkGraph(const std::vector<std::vector<int>> &g, int m, bool coverOracle = false) {
    int n = int(g.size());
    BipartiteMatching a(n, m);
    for (int x = 0; x < n; ++x)
        for (int y : g[x])
            a.add(x, y);
    int want = oracle(g, 0, 0);
    CHECK(a.work() == want);
    CHECK(a.work() == want);
    int cnt = 0;
    for (int x = 0; x < n; ++x) {
        int y = a.matchL[x];
        if (y != -1) {
            CHECK(0 <= y and y < m);
            CHECK(a.matchR[y] == x);
            CHECK(std::find(g[x].begin(), g[x].end(), y) != g[x].end());
            ++cnt;
        }
    }
    for (int y = 0; y < m; ++y)
        if (a.matchR[y] != -1)
            CHECK(a.matchL[a.matchR[y]] == y);
    CHECK(cnt == want);
    auto [left, right] = a.minCover();
    CHECK(int(left.size() + right.size()) == want);
    std::vector<bool> l(n), r(m);
    for (int x : left) {
        CHECK(0 <= x and x < n and !l[x]);
        l[x] = true;
    }
    for (int y : right) {
        CHECK(0 <= y and y < m and !r[y]);
        r[y] = true;
    }
    for (int x = 0; x < n; ++x)
        for (int y : g[x])
            CHECK(l[x] or r[y]);
    if (coverOracle) {
        int best = n + m;
        for (unsigned mask = 0; mask < (1U << (n + m)); ++mask) {
            bool ok = true;
            for (int x = 0; x < n; ++x)
                for (int y : g[x])
                    ok = ok and ((mask >> x & 1) or (mask >> (n + y) & 1));
            if (ok)
                best = std::min(best, __builtin_popcount(mask));
        }
        CHECK(best == want);
    }
}

int main() {
    runCase("BipartiteMatching/exhaustive", [] {
        for (int n = 0; n <= 3; ++n)
            for (int m = 0; m <= 3; ++m)
                for (unsigned mask = 0; mask < (1U << (n * m)); ++mask) {
                    test_context::step = int(mask);
                    std::vector<std::vector<int>> g(n);
                    for (int x = 0; x < n; ++x)
                        for (int y = 0; y < m; ++y)
                            if (mask >> (x * m + y) & 1)
                                g[x].push_back(y);
                    checkGraph(g, m, true);
                }
    });
    runCase("BipartiteMatching/random-parallel", [] {
        for (int it = 0; it < 120; ++it) {
            test_context::step = it;
            int n = randomInt(0, 7), m = randomInt(0, 7);
            std::vector<std::vector<int>> g(n);
            for (int x = 0; x < n; ++x)
                for (int y = 0; y < m; ++y) {
                    int copies = randomInt(0, 2);
                    for (int k = 0; k < copies; ++k)
                        g[x].push_back(y);
                }
            checkGraph(g, m);
        }
    });
    runCase("BipartiteMatching/incremental", [] {
        BipartiteMatching a(4, 4);
        CHECK(a.work() == 0);
        a.add(0, 0);
        a.add(0, 1);
        a.add(1, 0);
        CHECK(a.work() == 2);
        a.add(2, 1);
        a.add(1, 2);
        auto cover = a.minCover();
        CHECK(cover.first.size() + cover.second.size() == 3);
        CHECK(a.work() == 3);
        a.add(3, 2);
        a.add(2, 3);
        CHECK(a.work() == 4);
    });
    runCase("BipartiteMatching/deep-alternating-path", [] {
        const int n = 20000;
        BipartiteMatching a(n, n);
        for (int x = 0; x + 1 < n; ++x) {
            a.add(x, x);
            a.add(x, x + 1);
        }
        a.add(n - 1, 0);
        CHECK(a.work() == n);
        CHECK(a.matchL.back() == 0);
        for (int x = 0; x + 1 < n; ++x)
            CHECK(a.matchL[x] == x + 1);
    });
}
