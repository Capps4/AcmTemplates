#include "../../../../../src/extra/Graph/LowerBoundFlow/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

struct Edge {
    int s, t, lo, hi;
};

using Table = std::vector<std::vector<std::optional<int>>>;

std::pair<bool, Table> oracle(int n, const std::vector<Edge> &ed) {
    std::vector<int> bal(n);
    std::pair<bool, Table> res{false, Table(n, std::vector<std::optional<int>>(n))};
    auto visit = [&](auto &&self, int i) -> void {
        if (i == int(ed.size())) {
            int s = -1, t = -1;
            for (int x = 0; x < n; ++x) {
                if (bal[x] < 0) {
                    if (s != -1) {
                        return;
                    }
                    s = x;
                } else if (bal[x] > 0) {
                    if (t != -1) {
                        return;
                    }
                    t = x;
                }
            }
            if (s == -1) {
                CHECK(t == -1);
                res.first = true;
            } else {
                CHECK(t != -1 and bal[t] == -bal[s]);
                auto &best = res.second[s][t];
                if (!best or -bal[s] > *best) {
                    best = -bal[s];
                }
            }
            return;
        }
        auto [x, y, lo, hi] = ed[i];
        for (int f = lo; f <= hi; ++f) {
            bal[x] -= f;
            bal[y] += f;
            self(self, i + 1);
            bal[x] += f;
            bal[y] -= f;
        }
    };
    visit(visit, 0);
    if (res.first) {
        for (int x = 0; x < n; ++x) {
            for (int y = 0; y < n; ++y) {
                if (x != y and !res.second[x][y]) {
                    res.second[x][y] = 0;
                }
            }
        }
    }
    return res;
}

void checkFlows(const LowerBoundFlow<long long> &a, int n, const std::vector<Edge> &ed,
                int s, int t, long long val) {
    std::vector<long long> bal(n);
    for (int i = 0; i < int(ed.size()); ++i) {
        auto [x, y, lo, hi] = ed[i];
        auto f = a.get(i);
        CHECK(lo <= f and f <= hi);
        bal[x] -= f;
        bal[y] += f;
    }
    for (int x = 0; x < n; ++x)
        CHECK(bal[x] == (x == s ? -val : x == t ? val : 0));
}

void checkGraph(int n, const std::vector<Edge> &ed) {
    LowerBoundFlow<long long> a(n);
    for (int i = 0; i < int(ed.size()); ++i) {
        auto [x, y, lo, hi] = ed[i];
        CHECK(a.add(x, y, lo, hi) == i);
    }
    auto [circ, want] = oracle(n, ed);
    CHECK(a.work() == circ);
    if (circ)
        checkFlows(a, n, ed, -1, -1, 0);
    for (int s = 0; s < n; ++s)
        for (int t = 0; t < n; ++t)
            if (s != t) {
                auto got = a.work(s, t);
                CHECK(got.has_value() == want[s][t].has_value());
                if (got) {
                    CHECK(*got == *want[s][t]);
                    checkFlows(a, n, ed, s, t, *got);
                }
            }
}

int main() {
    runCase("LowerBoundFlow/exhaustive", [] {
        const std::vector<std::pair<int, int>> bounds{{0, 0}, {0, 1}, {1, 1}};
        for (int mask = 0; mask < 729; ++mask) {
            test_context::step = mask;
            int code = mask;
            std::vector<Edge> ed;
            for (int x = 0; x < 3; ++x)
                for (int y = 0; y < 3; ++y)
                    if (x != y) {
                        auto [lo, hi] = bounds[code % 3];
                        code /= 3;
                        ed.push_back({x, y, lo, hi});
                    }
            checkGraph(3, ed);
        }
    });
    runCase("LowerBoundFlow/random-parallel-loops", [] {
        checkGraph(0, {});
        checkGraph(1, {{0, 0, 2, 3}});
        checkGraph(2, {{1, 0, 1, 1}});
        for (int it = 0; it < 100; ++it) {
            test_context::step = it;
            int n = randomInt(1, 4), m = randomInt(0, 7);
            std::vector<Edge> ed;
            for (int i = 0; i < m; ++i) {
                int lo = randomInt(0, 2);
                ed.push_back({randomInt(0, n - 1), randomInt(0, n - 1), lo, lo + randomInt(0, 2)});
            }
            checkGraph(n, ed);
        }
    });
    runCase("LowerBoundFlow/rebuild-and-large-capacity", [] {
        LowerBoundFlow<long long> a(3);
        const auto lim = std::numeric_limits<long long>::max();
        a.add(0, 1, lim / 4, lim / 3);
        a.add(1, 2, lim / 4, lim / 3);
        CHECK(a.work(0, 2) == lim / 3);
        CHECK(a.get(0) == lim / 3 and a.get(1) == lim / 3);
        CHECK(a.work(0, 2) == lim / 3);
        CHECK(!a.work());
        a.add(2, 0, 0, lim / 3);
        CHECK(a.work());
        CHECK(a.get(0) == a.get(1) and a.get(1) == a.get(2));
        CHECK(a.work(0, 2) == lim / 3);
        LowerBoundFlow<long long> b(2);
        b.add(0, 1, lim, lim);
        CHECK(b.work(0, 1) == lim);
        CHECK(b.get(0) == lim);
    });
    runCase("LowerBoundFlow/wide-network", [] {
        const int n = 20000;
        LowerBoundFlow<long long> a(n + 2);
        for (int x = 0; x < n; ++x) {
            a.add(n, x, 1, 3);
            a.add(x, n + 1, 1, 2);
        }
        CHECK(a.work(n, n + 1) == 2LL * n);
        for (int i = 0; i < 2 * n; ++i)
            CHECK(a.get(i) == 2);
    });
}
