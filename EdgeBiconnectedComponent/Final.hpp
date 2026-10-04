#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

// SNIPPET BEGIN
class EdgeBc {
public:
    std::vector<int> dfn, low, bel, cutDeg;
    std::vector<std::vector<int>> g{};
    std::vector<std::pair<int, int>> bridges{};
    int cntBlock = 0, componentNum = 0;

    explicit EdgeBc(const std::vector<std::vector<int>> &adj)
        : dfn(adj.size(), -1), low(adj.size()), bel(adj.size(), -1), cutDeg(adj.size()) {
        struct Frame {
            int x, skipParent;
            int y = 0;
        };

        std::vector<Frame> stk;
        std::vector<int> q;
        stk.reserve(adj.size());
        q.reserve(adj.size());
        int cur = 0;
        auto enter = [&](int x, int fa) {
            dfn[x] = low[x] = cur++;
            q.push_back(x);
            stk.push_back({x, fa});
        };
        for (int root = 0; root < int(adj.size()); ++root) {
            if (dfn[root] != -1) continue;
            ++componentNum;
            enter(root, -1);
            while (!stk.empty()) {
                auto &fr = stk.back();
                int x = fr.x;
                if (fr.y < int(adj[x].size())) {
                    int y = adj[x][fr.y++];
                    assert(0 <= y && y < int(adj.size()));
                    if (y == fr.skipParent) {
                        fr.skipParent = -1;
                        continue;
                    }
                    if (dfn[y] == -1)
                        enter(y, x);
                    else
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
                int fa = stk.empty() ? -1 : stk.back().x;
                if (fa != -1) {
                    low[fa] = std::min(low[fa], low[x]);
                    if (low[x] > dfn[fa]) {
                        bridges.emplace_back(fa, x);
                        ++cutDeg[fa];
                        ++cutDeg[x];
                    }
                }
            }
        }
        g.resize(cntBlock);
        for (auto [x, y] : bridges) {
            g[bel[x]].push_back(bel[y]);
            g[bel[y]].push_back(bel[x]);
        }
    }
};

using EdgeBC = EdgeBc;
