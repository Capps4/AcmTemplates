#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

// SNIPPET BEGIN
class Scc {
public:
    std::vector<int> dfn, low, bel;
    std::vector<std::vector<int>> g{};
    int cntBlock = 0;

    explicit Scc(const std::vector<std::vector<int>> &adj, bool buildCondensation = true)
        : dfn(adj.size(), -1), low(adj.size()), bel(adj.size(), -1) {
        struct Frame {
            int x;
            int y = 0;
        };

        std::vector<Frame> stk;
        std::vector<int> q;
        stk.reserve(adj.size());
        q.reserve(adj.size());
        int cur = 0;
        auto enter = [&](int x) {
            dfn[x] = low[x] = cur++;
            q.push_back(x);
            stk.push_back({x});
        };
        for (int root = 0; root < int(adj.size()); ++root) {
            if (dfn[root] != -1) continue;
            enter(root);
            while (!stk.empty()) {
                auto &fr = stk.back();
                int x = fr.x;
                if (fr.y < int(adj[x].size())) {
                    int y = adj[x][fr.y++];
                    assert(0 <= y && y < int(adj.size()));
                    if (dfn[y] == -1)
                        enter(y);
                    else if (bel[y] == -1)
                        low[x] = std::min(low[x], dfn[y]);
                    continue;
                }
                if (dfn[x] == low[x]) {
                    int y;
                    do {
                        y = q.back();
                        q.pop_back();
                        bel[y] = cntBlock;
                    } while (y != x);
                    ++cntBlock;
                }
                stk.pop_back();
                if (!stk.empty()) {
                    int fa = stk.back().x;
                    low[fa] = std::min(low[fa], low[x]);
                }
            }
        }
        if (buildCondensation) {
            g.resize(cntBlock);
            for (int x = 0; x < int(adj.size()); ++x)
                for (int y : adj[x])
                    if (bel[x] != bel[y]) g[bel[x]].push_back(bel[y]);
            // 如需缩点图去重，启用下面四行。
            // for (auto &edges : g) {
            //     std::sort(edges.begin(), edges.end());
            //     edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
            // }
        }
    }
};

using SCC = Scc;
