#pragma once
#include <cassert>
#include <utility>
#include <vector>

// SNIPPET BEGIN
class GridUtil {
    static constexpr int dx[4] = {0, 0, -1, 1};
    static constexpr int dy[4] = {-1, 1, 0, 0};

public:
    int n, m;

    GridUtil(int n, int m) : n(n), m(m) { assert(n >= 0 && m >= 0); }

    constexpr bool contains(int x, int y) const { return 0 <= x && x < n && 0 <= y && y < m; }

    // Calls f(nx, ny) in left/right/up/down order without allocation.
    template <class F>
    void forEachNeighbor(int x, int y, F &&f) const {
        assert(contains(x, y));
        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k], ny = y + dy[k];
            if (contains(nx, ny)) f(nx, ny);
        }
    }

    std::vector<std::pair<int, int>> neighbors(int x, int y) const {
        std::vector<std::pair<int, int>> res;
        res.reserve(4);
        forEachNeighbor(x, y, [&](int nx, int ny) { res.emplace_back(nx, ny); });
        return res;
    }
};
