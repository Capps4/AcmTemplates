#include "../../../../../src/extra/Graph/GeneralMatching/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int oracle(const std::vector<unsigned> &g, unsigned mask, std::vector<int> &dp) {
    if (mask == 0) {
        return 0;
    }
    if (dp[mask] != -1) {
        return dp[mask];
    }
    int x = __builtin_ctz(mask);
    unsigned rest = mask ^ (1U << x);
    int ans = oracle(g, rest, dp);
    for (unsigned s = g[x] & rest; s; s &= s - 1) {
        int y = __builtin_ctz(s);
        ans = std::max(ans, 1 + oracle(g, rest ^ (1U << y), dp));
    }
    return dp[mask] = ans;
}

void checkGraph(int n, const std::vector<std::pair<int, int>> &ed) {
    GeneralMatching a(n);
    std::vector<unsigned> g(n);
    for (auto [x, y] : ed) {
        a.add(x, y);
        if (x != y) {
            g[x] |= 1U << y;
            g[y] |= 1U << x;
        }
    }
    std::vector<int> dp(1U << n, -1);
    int want = oracle(g, (1U << n) - 1, dp);
    CHECK(a.work() == want);
    CHECK(a.work() == want);
    int cnt = 0;
    for (int x = 0; x < n; ++x) {
        int y = a.mate[x];
        if (y != -1) {
            CHECK(0 <= y and y < n and x != y);
            CHECK(a.mate[y] == x and (g[x] >> y & 1));
            ++cnt;
        }
    }
    CHECK(cnt == 2 * want);
}

void checkAllGraphs(int n) {
    int m = n * (n - 1) / 2;
    for (unsigned mask = 0; mask < (1U << m); ++mask) {
        test_context::step = int(mask);
        std::vector<std::pair<int, int>> ed;
        int id = 0;
        for (int x = 0; x < n; ++x) {
            for (int y = x + 1; y < n; ++y) {
                if (mask >> id++ & 1) {
                    ed.emplace_back(x, y);
                }
            }
        }
        checkGraph(n, ed);
    }
}

int main() {
    runCase("GeneralMatching/exhaustive", [] {
        for (int n = 0; n <= 5; ++n) {
            checkAllGraphs(n);
        }
    });
    runStressCase("GeneralMatching/exhaustive-six-vertices", [] { checkAllGraphs(6); });
    runCase("GeneralMatching/random-multigraph", [] {
        for (int it = 0; it < 500; ++it) {
            test_context::step = it;
            int n = randomInt(1, 12), m = randomInt(0, 50);
            std::vector<std::pair<int, int>> ed;
            for (int i = 0; i < m; ++i) {
                ed.emplace_back(randomInt(0, n - 1), randomInt(0, n - 1));
            }
            checkGraph(n, ed);
        }
    });
    runCase("GeneralMatching/blossoms-and-incremental", [] {
        checkGraph(10, {{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 5},
                        {5, 6}, {6, 2}, {0, 7}, {4, 8}, {6, 9}, {7, 8}});
        GeneralMatching a(4);
        a.add(0, 1);
        a.add(1, 2);
        a.add(2, 0);
        CHECK(a.work() == 1);
        a.add(1, 3);
        CHECK(a.work() == 2);
    });
    runCase("GeneralMatching/deep-alternating-path", [] {
        int n = 2000;
        GeneralMatching a(n);
        for (int x = 1; x + 1 < n; x += 2) {
            a.add(x, x + 1);
        }
        CHECK(a.work() == n / 2 - 1);
        for (int x = 0; x + 1 < n; x += 2) {
            a.add(x, x + 1);
        }
        CHECK(a.work() == n / 2);
        CHECK(a.mate[0] == 1 and a.mate[n - 1] == n - 2);
    });
}
