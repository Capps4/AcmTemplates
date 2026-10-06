#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
class VertexBC {
    const int n;
    const std::vector<std::vector<int>> &adj;
    std::stack<int, std::vector<int>> q{};
    int cur = 0, sqid;

    void dfs(int x) {
        dfn[x] = low[x] = cur++;
        q.push(x);
        for (int y : adj[x]) {
            if (dfn[y] == -1) {
                dfs(y);
                low[x] = std::min(low[x], low[y]);
                if (low[y] == dfn[x]) {
                    csqt.push_back({});
                    int z;
                    do {
                        z = q.top();
                        q.pop();
                        csqt[z].push_back(sqid);
                        csqt[sqid].push_back(z);
                    } while (z != y);
                    csqt[x].push_back(sqid);
                    csqt[sqid++].push_back(x);
                }
            } else {
                low[x] = std::min(low[x], dfn[y]);
            }
        }
    }

public:
    // original graph
    std::vector<int> dfn, low;
    std::vector<std::vector<int>> csqt; // 圆方树
    int componentNum = 0;

    VertexBC(const std::vector<std::vector<int>> &adj)
        : n(adj.size()),
          adj(adj),
          sqid{n},
          dfn(n, -1),
          low(n),
          csqt(n) {

        for (int i = 0; i < n; i++) {
            if (dfn[i] == -1) {
                componentNum += 1;
                dfs(i);
                q.pop();
            }
        }
    }
};

