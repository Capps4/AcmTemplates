#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/GraphTheory/GridUtil/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <memory>


int coreCases() {
    for (int n = 0; n <= 4; ++n) for (int m = 0; m <= 4; ++m) {
        GridUtil grid(n, m);
        CHECK(!grid.contains(-1, 0) && !grid.contains(n, 0));
        CHECK(!grid.contains(0, -1) && !grid.contains(0, m));
        for (int x = 0; x < n; ++x) for (int y = 0; y < m; ++y) {
            std::vector<std::pair<int, int>> expected;
            for (auto [dx, dy] : {std::pair{-0, -1}, std::pair{0, 1}, std::pair{-1, 0}, std::pair{1, 0}})
                if (0 <= x + dx && x + dx < n && 0 <= y + dy && y + dy < m)
                    expected.emplace_back(x + dx, y + dy);
            CHECK(grid.neighbors(x, y) == expected);
            std::vector<std::pair<int, int>> actual;
            grid.forEachNeighbor(x, y, [state = std::make_unique<int>(0), &actual](int nx, int ny) {
                ++*state;
                actual.emplace_back(nx, ny);
            });
            CHECK(actual == expected);
        }
    }
    std::cout << "All cells in 0..20 grids, direction order and move-only callback passed\n";
    return 0;
}

#include "../../../../src/GraphTheory/GridUtil/code.hpp"
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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

int run() {
    runCase("GridUtil/zero-rows", [] {
        verifyAdded(0, 7);
    });
    runCase("GridUtil/zero-columns", [] {
        verifyAdded(8, 0);
    });
    runCase("GridUtil/single", [] {
        verifyAdded(1, 1);
    });
    runCase("GridUtil/row", [] {
        verifyAdded(1, 9);
    });
    runCase("GridUtil/column", [] {
        verifyAdded(11, 1);
    });
    runCase("GridUtil/square", [] {
        verifyAdded(2, 2);
    });
    runCase("GridUtil/rectangle", [] {
        verifyAdded(13, 7);
    });
    return 0;
}
}

int main() {
    runCase("GridUtil/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
