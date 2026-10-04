#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

// SNIPPET BEGIN
class VertexBc {
public:
    std::vector<int> dfn, low;
    std::vector<std::vector<int>> csqt{};
    std::vector<bool> isCut;
    int componentNum = 0;

    explicit VertexBc(const std::vector<std::vector<int>> &adj)
        : dfn(adj.size(), -1), low(adj.size()), isCut(adj.size()) {
        int n = int(adj.size());
        csqt.reserve(2 * n);
        csqt.resize(n);

        struct Frame {
            int x, skipParent;
            int y = 0;
        };

        std::vector<Frame> stk;
        std::vector<int> q;
        stk.reserve(n);
        q.reserve(n);
        int cur = 0;
        auto enter = [&](int x, int fa) {
            dfn[x] = low[x] = cur++;
            q.push_back(x);
            stk.push_back({x, fa});
        };
        for (int root = 0; root < n; ++root) {
            if (dfn[root] != -1) continue;
            ++componentNum;
            enter(root, -1);
            while (!stk.empty()) {
                auto &fr = stk.back();
                int x = fr.x;
                if (fr.y < int(adj[x].size())) {
                    int y = adj[x][fr.y++];
                    assert(0 <= y && y < n);
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
                stk.pop_back();
                int fa = stk.empty() ? -1 : stk.back().x;
                if (fa == -1) {
                    q.pop_back();
                    continue;
                }
                low[fa] = std::min(low[fa], low[x]);
                if (low[x] >= dfn[fa]) {
                    int blk = int(csqt.size());
                    csqt.emplace_back();
                    int y;
                    do {
                        y = q.back();
                        q.pop_back();
                        csqt[y].push_back(blk);
                        csqt[blk].push_back(y);
                    } while (y != x);
                    csqt[fa].push_back(blk);
                    csqt[blk].push_back(fa);
                }
            }
        }
        for (int x = 0; x < n; ++x)
            isCut[x] = csqt[x].size() > 1;
    }
};

using VertexBC = VertexBc;
