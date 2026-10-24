#pragma once
#include "Include.hpp"
class TwoSat {
    const int n;
    std::vector<std::vector<int>> adj;
public:
    std::vector<bool> ans;
    TwoSat(int n) : n(n), adj(2 * n), ans(n) {}
    // x * 2         为 x 设 0
    // x * 2 + 1     为 x 设 1

    // 如果 x 取 f 那么 y 要取 g
    void add(int x, bool f, int y, bool g) {
        x *= 2, y *= 2;
        adj[x + f].push_back(y + g);
        adj[y + !g].push_back(x + !f);
    }

    void assign(int x, bool f) {
        x *= 2;
        adj[x + !f].push_back(x + f);
    }

    bool work() {
        ans.assign(n, false);
        SCC scc(adj);
        const auto &bel = scc.bel;

        for (int i = 0; i < n; i++) {
            if (bel[2 * i] == bel[2 * i + 1]) {
                ans.clear();
                return false;
            }
            ans[i] = bel[2 * i] > bel[2 * i + 1];
        }

        return true;
    }
};

