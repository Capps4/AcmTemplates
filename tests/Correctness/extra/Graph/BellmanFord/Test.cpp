#include "../../../../../src/extra/Graph/BellmanFord/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using i64 = long long;
using Edge = std::tuple<int, int, i64>;

void checkGraph(int n, const std::vector<Edge> &ed) {
    using BF = BellmanFord<i64>;
    const __int128_t inf = __int128_t(1) << 120;
    std::vector<std::vector<__int128_t>> d(n, std::vector<__int128_t>(n, inf));
    BF a(n);
    for (int x = 0; x < n; ++x) {
        d[x][x] = 0;
    }
    for (int id = 0; id < int(ed.size()); ++id) {
        auto [x, y, w] = ed[id];
        CHECK(a.add(x, y, w) == id);
        d[x][y] = std::min(d[x][y], __int128_t(w));
    }
    for (int k = 0; k < n; ++k) {
        for (int x = 0; x < n; ++x) {
            for (int y = 0; y < n; ++y) {
                if (d[x][k] != inf and d[k][y] != inf) {
                    d[x][y] = std::min(d[x][y], d[x][k] + d[k][y]);
                }
            }
        }
    }
    for (int s = 0; s < n; ++s) {
        auto r = a.work(s);
        bool neg = false;
        for (int x = 0; x < n; ++x) {
            auto want = d[s][x] == inf ? BF::Unreachable : BF::Finite;
            for (int k = 0; k < n; ++k) {
                if (d[s][k] != inf and d[k][k] < 0 and d[k][x] != inf) {
                    want = BF::NegativeInfinity;
                }
            }
            CHECK(r.state[x] == want);
            neg = neg or want == BF::NegativeInfinity;
            if (want == BF::Finite) {
                CHECK(r.dis[x] == d[s][x]);
                int y = x, len = 0;
                __int128_t sum = 0;
                while (y != s) {
                    CHECK(++len < n);
                    int id = r.pre[y];
                    CHECK(0 <= id and id < int(ed.size()));
                    auto [u, v, w] = ed[id];
                    CHECK(v == y);
                    sum += w;
                    y = u;
                }
                CHECK(sum == r.dis[x]);
            } else if (want == BF::Unreachable) {
                CHECK(r.pre[x] == -1);
            }
        }
        CHECK(!r.cycle.empty() == neg);
        if (neg) {
            __int128_t sum = 0;
            for (int i = 0; i < int(r.cycle.size()); ++i) {
                int id = r.cycle[i], nxt = r.cycle[(i + 1) % r.cycle.size()];
                CHECK(0 <= id and id < int(ed.size()));
                CHECK(std::get<1>(ed[id]) == std::get<0>(ed[nxt]));
                CHECK(d[s][std::get<0>(ed[id])] != inf);
                sum += std::get<2>(ed[id]);
            }
            CHECK(sum < 0);
        }
    }
}

int main() {
    runCase("BellmanFord/exhaustive", [] {
        for (int mask = 0; mask < 4096; ++mask) {
            test_context::step = mask;
            int bits = mask;
            std::vector<Edge> ed;
            for (int x = 0; x < 3; ++x) {
                for (int y = 0; y < 3; ++y) {
                    if (x != y) {
                        int k = bits % 4;
                        bits /= 4;
                        if (k != 0) {
                            ed.emplace_back(x, y, k - 2);
                        }
                    }
                }
            }
            checkGraph(3, ed);
        }
    });
    runCase("BellmanFord/random-multigraph", [] {
        for (int it = 0; it < 400; ++it) {
            test_context::step = it;
            int n = randomInt(1, 7), m = randomInt(0, 20);
            std::vector<Edge> ed;
            for (int i = 0; i < m; ++i) {
                ed.emplace_back(randomInt(0, n - 1), randomInt(0, n - 1), randomInt(-8, 8));
            }
            checkGraph(n, ed);
        }
    });
    runCase("BellmanFord/limits-and-negative-descendants", [] {
        i64 lo = std::numeric_limits<i64>::min(), hi = std::numeric_limits<i64>::max();
        checkGraph(1, {});
        checkGraph(1, {{0, 0, lo}});
        checkGraph(4, {{0, 1, hi}, {1, 2, hi}, {2, 3, lo}});
        checkGraph(4, {{0, 1, lo}, {1, 2, lo}, {2, 3, hi}});
        checkGraph(7, {{0, 1, 3}, {1, 2, -2}, {2, 1, 1}, {2, 3, 1},
                       {0, 4, 9}, {5, 6, -2}, {6, 5, 1}});
        BellmanFord<int> a(2);
        a.add(0, 1, 5);
        CHECK(a.work(0).dis[1] == 5);
        a.add(0, 1, -3);
        CHECK(a.work(0).dis[1] == -3);
    });
    runCase("BellmanFord/deep-chain", [] {
        int n = 20000;
        BellmanFord<int> a(n);
        for (int x = 1; x < n; ++x) {
            a.add(x - 1, x, -1);
        }
        auto r = a.work(0);
        CHECK(r.cycle.empty());
        for (int x = 0; x < n; ++x) {
            CHECK(r.state[x] == BellmanFord<int>::Finite and r.dis[x] == -x);
        }
    });
}
