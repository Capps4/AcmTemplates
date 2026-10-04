#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
int main() {
    Legacy::GridUtil oldGrid(500, 500);
    GridUtil grid(500, 500);
    auto before = [&] {
        std::uint64_t sum = 0;
        for (int x = 0; x < 500; ++x) for (int y = 0; y < 500; ++y)
            for (auto [nx, ny] : oldGrid.neighbors(x, y)) sum += nx * 500 + ny;
        return sum;
    };
    compare("owning-neighbors", before, [&] {
        std::uint64_t sum = 0;
        for (int x = 0; x < 500; ++x) for (int y = 0; y < 500; ++y)
            for (auto [nx, ny] : grid.neighbors(x, y)) sum += nx * 500 + ny;
        return sum;
    });
    compare("callback-neighbors", before, [&] {
        std::uint64_t sum = 0;
        for (int x = 0; x < 500; ++x) for (int y = 0; y < 500; ++y)
            grid.forEachNeighbor(x, y, [&](int nx, int ny) { sum += nx * 500 + ny; });
        return sum;
    });
}
