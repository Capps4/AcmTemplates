#include "../../../../../src/extra/Graph/DirectedMST/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using i64 = long long;
using Edge = std::tuple<int, int, i64>;

std::optional<__int128_t> oracle(int n, int root, const std::vector<Edge> &ed) {
    std::vector<std::vector<int>> inc(n);
    for (int id = 0; id < int(ed.size()); ++id) {
        auto [x, y, w] = ed[id];
        (void)w;
        if (x != y and y != root) {
            inc[y].push_back(id);
        }
    }
    std::optional<__int128_t> ans;
    std::vector<int> pre(n, -1);
    auto dfs = [&](auto &self, int x, __int128_t sum) -> void {
        if (x == n) {
            for (int s = 0; s < n; ++s) {
                int y = s, len = 0;
                while (y != root and len < n) {
                    y = pre[y];
                    ++len;
                }
                if (y != root) {
                    return;
                }
            }
            if (!ans or sum < *ans) {
                ans = sum;
            }
        } else if (x == root) {
            self(self, x + 1, sum);
        } else {
            for (int id : inc[x]) {
                pre[x] = std::get<0>(ed[id]);
                self(self, x + 1, sum + std::get<2>(ed[id]));
            }
        }
    };
    dfs(dfs, 0, 0);
    return ans;
}

void checkGraph(int n, const std::vector<Edge> &ed) {
    DirectedMST<i64> a(n);
    for (int id = 0; id < int(ed.size()); ++id) {
        auto [x, y, w] = ed[id];
        CHECK(a.add(x, y, w) == id);
    }
    for (int root = 0; root < n; ++root) {
        auto want = oracle(n, root, ed), r = std::optional<__int128_t>{};
        auto got = a.work(root);
        if (got) {
            r = got->cost;
        }
        CHECK(r == want);
        if (!got) {
            continue;
        }
        CHECK(int(got->edges.size()) == n - 1);
        std::vector<int> pre(n, -1);
        __int128_t sum = 0;
        for (int id : got->edges) {
            CHECK(0 <= id and id < int(ed.size()));
            auto [x, y, w] = ed[id];
            CHECK(y != root and x != y and pre[y] == -1);
            pre[y] = x;
            sum += w;
        }
        CHECK(sum == got->cost);
        for (int s = 0; s < n; ++s) {
            int x = s, len = 0;
            while (x != root) {
                CHECK(++len < n);
                CHECK(pre[x] != -1);
                x = pre[x];
            }
        }
    }
}

void checkDeep(int n) {
    DirectedMST<i64> a(n);
    a.add(0, 1, n);
    for (int x = 2; x < n; ++x) {
        a.add(x - 1, x, 0);
        a.add(x, 1, x - 2);
    }
    auto r = a.work(0);
    CHECK(r and r->cost == n and int(r->edges.size()) == n - 1);
    for (int x = 1; x < n; ++x) {
        CHECK(r->edges[x - 1] == (x == 1 ? 0 : 2 * x - 3));
    }
}

int main() {
    runCase("DirectedMST/exhaustive", [] {
        for (int mask = 0; mask < 4096; ++mask) {
            test_context::step = mask;
            int bits = mask;
            std::vector<Edge> ed;
            for (int x = 0; x < 3; ++x) {
                for (int y = 0; y < 3; ++y) {
                    if (x != y) {
                        int k = bits % 4;
                        bits /= 4;
                        if (k) {
                            ed.emplace_back(x, y, k - 2);
                        }
                    }
                }
            }
            checkGraph(3, ed);
        }
    });
    runCase("DirectedMST/random-multigraph", [] {
        for (int it = 0; it < 500; ++it) {
            test_context::step = it;
            int n = randomInt(1, 6), m = randomInt(0, 14);
            std::vector<Edge> ed;
            for (int i = 0; i < m; ++i) {
                ed.emplace_back(randomInt(0, n - 1), randomInt(0, n - 1), randomInt(-9, 9));
            }
            checkGraph(n, ed);
        }
    });
    runCase("DirectedMST/limits-and-nested-cycles", [] {
        i64 lo = std::numeric_limits<i64>::min(), hi = std::numeric_limits<i64>::max();
        checkGraph(1, {{0, 0, lo}});
        checkGraph(4, {{0, 1, hi}, {1, 2, hi}, {2, 3, hi}});
        checkGraph(4, {{0, 1, lo}, {1, 2, lo}, {2, 3, lo}});
        checkGraph(4, {{0, 1, hi}, {1, 2, lo}, {2, 1, lo}, {2, 3, hi}, {3, 1, 0}});
        checkGraph(6, {{0, 1, 20}, {1, 2, 0}, {2, 1, 0}, {2, 3, 1},
                       {3, 4, 0}, {4, 3, 0}, {4, 1, 1}, {4, 5, 1}, {5, 1, 3}});
        DirectedMST<int> a(2);
        CHECK(!a.work(0));
        a.add(0, 1, -1);
        CHECK(a.work(0)->cost == -1);
        a.add(0, 1, -2);
        CHECK(a.work(0)->cost == -2);
        CHECK(a.work(0)->edges == std::vector<int>{1});
    });
    runCase("DirectedMST/deep-contraction", [] { checkDeep(5000); });
    runStressCase("DirectedMST/deep-contraction-20k", [] { checkDeep(20000); });
}
