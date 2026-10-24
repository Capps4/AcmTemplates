#pragma once
#include "Include.hpp"

class OfflineDynamicConnectivity {
    int n, q, b = 1;
    std::vector<std::vector<std::pair<int, int>>> seg{};

public:
    OfflineDynamicConnectivity(int n, int q) : n(n), q(q) {
        assert(n >= 0 and 0 <= q and q <= std::numeric_limits<int>::max() / 2);
        while (b < q) {
            b *= 2;
        }
        seg.resize(std::size_t(b) * 2);
    }

    void add(int l, int r, int x, int y) {
        assert(0 <= l and l <= r and r <= q);
        assert(0 <= x and x < n and 0 <= y and y < n);
        l += b;
        r += b;
        while (l < r) {
            if (l & 1) {
                seg[l++].emplace_back(x, y);
            }
            if (r & 1) {
                seg[--r].emplace_back(x, y);
            }
            l /= 2;
            r /= 2;
        }
    }

    template <class F>
    void work(F f) const {
        if (q == 0) {
            return;
        }
        RollbackDSU dsu(n);
        auto dfs = [&](auto &self, int x) -> void {
            auto s = dsu.snapshot();
            for (auto [u, v] : seg[x]) {
                dsu.Union(u, v);
            }
            if (x >= b) {
                if (x - b < q) {
                    f(x - b, std::as_const(dsu));
                }
            } else {
                self(self, x * 2);
                self(self, x * 2 + 1);
            }
            dsu.rollback(s);
        };
        dfs(dfs, 1);
    }
};
