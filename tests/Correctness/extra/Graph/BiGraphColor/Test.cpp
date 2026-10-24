#include "../../../../../src/extra/Graph/BiGraphColor/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

void check(const std::vector<std::vector<int>>& g) {
    int n = int(g.size());
    bool want = false;
    for (int s = 0; s < (1 << n); ++s) {
        bool ok = true;
        for (int x = 0; x < n; ++x) {
            for (int y : g[x]) {
                ok = ok and (((s >> x) ^ (s >> y)) & 1);
            }
        }
        want = want or ok;
    }
    BiGraphColor a(g);
    CHECK(a.isBiGraph == want);
    if (want) {
        for (int x = 0; x < n; ++x) {
            CHECK(a.col[x] == 0 or a.col[x] == 1);
            for (int y : g[x]) {
                CHECK(a.col[x] != a.col[y]);
            }
        }
    }
}

int main() {
    runCase("BiGraphColor/oracle", [] {
        check({});
        check({{}, {}, {}});
        check({{0}});
        check({{1, 1}, {0, 0}});
        check({{1, 2}, {0, 2}, {0, 1}});
        check({{1, 3}, {0, 2}, {1, 3}, {0, 2}});
        check({{1}, {0}, {3, 4}, {2, 4}, {2, 3}});
        for (int it = 0; it < 100; ++it) {
            int n = randomInt(0, 8);
            std::vector<std::vector<int>> g(n);
            for (int x = 0; x < n; ++x) {
                for (int y = x; y < n; ++y) {
                    if (randomInt(0, 5) == 0) {
                        g[x].push_back(y);
                        g[y].push_back(x);
                    }
                }
            }
            check(g);
        }
    });
}
