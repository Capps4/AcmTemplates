#include "../../../../src/GraphTheory/GridUtil/code.hpp"
#include "../../../Support/CaseSupport.hpp"
void verifyAdded(int n, int m) {
    GridUtil g(n, m);
    CHECK(!g.contains(-1, 0) and !g.contains(n, m));
    for (int x = 0; x < n; ++x)
        for (int y = 0; y < m; ++y) {
            std::vector<std::pair<int, int>> e, a;
            if (y > 0)
                e.push_back({x, y - 1});
            if (y + 1 < m)
                e.push_back({x, y + 1});
            if (x > 0)
                e.push_back({x - 1, y});
            if (x + 1 < n)
                e.push_back({x + 1, y});
            CHECK(g.neighbors(x, y) == e);
            g.forEachNeighbor(x, y, [&](int u, int v) {
                a.push_back({u, v});
            });
            CHECK(a == e);
        }
}

int main() {
    runCase("GridUtil/01-zero-rows", [] {
        verifyAdded(0, 7);
    });
    runCase("GridUtil/02-zero-columns", [] {
        verifyAdded(8, 0);
    });
    runCase("GridUtil/03-one-cell", [] {
        verifyAdded(1, 1);
    });
    runCase("GridUtil/04-one-row", [] {
        verifyAdded(1, 9);
    });
    runCase("GridUtil/05-one-column", [] {
        verifyAdded(11, 1);
    });
    runCase("GridUtil/06-two-by-two", [] {
        verifyAdded(2, 2);
    });
    runCase("GridUtil/07-wide", [] {
        verifyAdded(3, 17);
    });
    runCase("GridUtil/08-tall", [] {
        verifyAdded(19, 2);
    });
    runCase("GridUtil/09-square", [] {
        verifyAdded(8, 8);
    });
    runCase("GridUtil/10-non-square", [] {
        verifyAdded(13, 7);
    });
    return finishCases(10);
}
