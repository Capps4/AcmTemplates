#pragma once
#include <bits/stdc++.h>
using i64 = long long;
class EdgeBC {
    const int n;
    const std::vector<std::vector<int>> &adj;
    std::vector<int> q{}; // stack
    int r = 0, cur = 0;

    void dfs(int x, int fa) {
        dfn[x] = low[x] = cur++;
        q[++r] = x;

        for (int y : adj[x]) {
            if (y == fa) {
                fa = ~fa;
                continue;
            }
            if (dfn[y] == -1) {
                dfs(y, x);
                low[x] = std::min(low[x], low[y]);
            } else {
                low[x] = std::min(low[x], dfn[y]);
            }
        }

        if (dfn[x] == low[x]) {
            int y;
            do {
                y = q[r--];
                bel[y] = cntBlock;
            } while (y != x);
            cntBlock += 1;
        }
    }

public:
    // original graph
    std::vector<int> dfn, low, bel, cutDeg;

    // shrinking graph
    std::vector<std::vector<int>> g{};
    int cntBlock = 0, componentNum = 0;

    EdgeBC(const std::vector<std::vector<int>> &adj)
        : n(adj.size()), adj(adj), dfn(n, -1), low(n), bel(n, -1), cutDeg(n) {
        q.assign(n + 1, 0);

        for (int i = 0; i < n; i++) {
            if (dfn[i] == -1) {
                componentNum += 1;
                dfs(i, -1);
            }
        }

        g.resize(cntBlock);
        for (int x = 0; x < n; x++) {
            for (int y : adj[x]) {
                if (bel[x] == bel[y])
                    continue;
                g[bel[x]].push_back(bel[y]);
            }
        }
    }
};

