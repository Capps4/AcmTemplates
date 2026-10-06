#include "../../../../src/GraphTheory/GridUtil/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <memory>

int main() {
    for (int n = 0; n <= 20; ++n) for (int m = 0; m <= 20; ++m) {
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
}
