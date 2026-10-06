#pragma once
#include "../../../Headers/Headers.hpp"

// SNIPPET BEGIN
class GridUtil {
    static constexpr int D = 4;
    static constexpr int dx[D] = {0, 0, -1, 1};
    static constexpr int dy[D] = {-1, 1, 0, 0};

public:
    int n, m;

    GridUtil(int n, int m) : n(n), m(m) {
        assert(n >= 0 and m >= 0);
    }

    constexpr bool contains(int x, int y) const {
        return 0 <= x and x < n and 0 <= y and y < m;
    }

    // Calls f(nx, ny) in left/right/up/down order without allocation.
    template <class F>
    void forEachNeighbor(int x, int y, F &&f) const {
        assert(contains(x, y));
        for (int k = 0; k < D; ++k) {
            int nx = x + dx[k], ny = y + dy[k];
            if (contains(nx, ny))
                f(nx, ny);
        }
    }

    std::vector<std::pair<int, int>> neighbors(int x, int y) const {
        std::vector<std::pair<int, int>> res;
        res.reserve(D);
        forEachNeighbor(x, y, [&](int nx, int ny) {
            res.emplace_back(nx, ny);
        });
        return res;
    }
};
