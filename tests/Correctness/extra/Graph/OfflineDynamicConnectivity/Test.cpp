#include "../../../../../src/extra/Graph/OfflineDynamicConnectivity/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using Edge = std::array<int, 4>;

void checkTimeline(int n, int q, const std::vector<Edge> &ed, int reps = 1) {
    OfflineDynamicConnectivity a(n, q);
    for (auto [l, r, x, y] : ed) {
        a.add(l, r, x, y);
    }
    std::vector<std::vector<int>> ids(q, std::vector<int>(n, -1));
    std::vector<std::vector<int>> siz(q, std::vector<int>(n));
    for (int t = 0; t < q; ++t) {
        std::vector<std::vector<int>> g(n);
        for (auto [l, r, x, y] : ed) {
            if (l <= t and t < r) {
                g[x].push_back(y);
                g[y].push_back(x);
            }
        }
        for (int s = 0; s < n; ++s) {
            if (ids[t][s] != -1) {
                continue;
            }
            std::vector<int> bfs{s};
            ids[t][s] = s;
            for (int i = 0; i < int(bfs.size()); ++i) {
                for (int y : g[bfs[i]]) {
                    if (ids[t][y] == -1) {
                        ids[t][y] = s;
                        bfs.push_back(y);
                    }
                }
            }
            for (int x : bfs) {
                siz[t][x] = int(bfs.size());
            }
        }
    }
    for (int rep = 0; rep < reps; ++rep) {
        int cnt = 0;
        a.work([&](int t, const RollbackDSU &d) {
            CHECK(t == cnt++);
            for (int s = 0; s < n; ++s) {
                CHECK(d.size(s) == siz[t][s]);
                for (int x = 0; x < n; ++x) {
                    CHECK(d.same(s, x) == (ids[t][s] == ids[t][x]));
                }
            }
        });
        CHECK(cnt == q);
    }
}

int main() {
    runCase("OfflineDynamicConnectivity/boundaries", [] {
        checkTimeline(0, 0, {});
        checkTimeline(0, 5, {});
        checkTimeline(4, 0, {{0, 0, 1, 2}});
        checkTimeline(4, 7, {{0, 7, 0, 1}, {0, 3, 1, 2}, {3, 7, 2, 1},
                            {1, 6, 0, 1}, {3, 3, 2, 3}, {4, 5, 3, 3}, {6, 7, 2, 3}}, 2);
        OfflineDynamicConnectivity a(2, 3);
        a.work([](int, const RollbackDSU &d) { CHECK(!d.same(0, 1)); });
        a.add(1, 2, 0, 1);
        a.work([](int t, const RollbackDSU &d) { CHECK(d.same(0, 1) == (t == 1)); });
    });
    runCase("OfflineDynamicConnectivity/exhaustive-intervals", [] {
        for (int l = 0; l <= 4; ++l) {
            for (int r = l; r <= 4; ++r) {
                for (int a = 0; a <= 4; ++a) {
                    for (int b = a; b <= 4; ++b) {
                        checkTimeline(3, 4, {{l, r, 0, 1}, {a, b, 1, 2}});
                    }
                }
            }
        }
    });
    runCase("OfflineDynamicConnectivity/random-multigraph", [] {
        for (int it = 0; it < 300; ++it) {
            test_context::step = it;
            int n = randomInt(1, 8), q = randomInt(0, 24), m = randomInt(0, 30);
            std::vector<Edge> ed;
            for (int i = 0; i < m; ++i) {
                int l = randomInt(0, q), r = randomInt(l, q);
                ed.push_back({l, r, randomInt(0, n - 1), randomInt(0, n - 1)});
            }
            checkTimeline(n, q, ed);
        }
    });
    runCase("OfflineDynamicConnectivity/long-timeline", [] {
        int n = 20000;
        OfflineDynamicConnectivity a(n, n);
        for (int x = 1; x < n; ++x) {
            a.add(x, n, x - 1, x);
        }
        a.work([](int t, const RollbackDSU &d) { CHECK(d.size(0) == t + 1); });
    });
}
