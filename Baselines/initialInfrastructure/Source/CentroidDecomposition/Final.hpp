#pragma once
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

// SNIPPET BEGIN
class CentroidDecomposition {
public:
    std::vector<int> dfsOrder{};
    // std::vector<std::vector<int>> cdt{}; // 点分树；同时启用下面的相关注释。

    explicit CentroidDecomposition(const std::vector<std::vector<int>> &adj) {
        int n = int(adj.size());
        // cdt.resize(n);
        // std::vector<int> fa(n, -1);
        std::vector<char> removed(n);
        std::vector<int> parent(n), size(n), nodes;
        std::vector<int> q;
        nodes.reserve(n);
        q.reserve(n);
        dfsOrder.reserve(n);
        for (int root = 0; root < n; ++root) {
            if (removed[root]) continue;
            q.push_back(root);
            while (!q.empty()) {
                int s = q.back();
                q.pop_back();
                nodes.clear();
                nodes.push_back(s);
                parent[s] = -1;
                size[s] = 1;
                for (int i = 0; i < int(nodes.size()); ++i) {
                    int x = nodes[i];
                    for (int y : adj[x]) {
                        assert(0 <= y && y < n);
                        if (removed[y] || y == parent[x]) continue;
                        assert(nodes.size() < adj.size()); // Input must be a forest.
                        parent[y] = x;
                        size[y] = 1;
                        nodes.push_back(y);
                    }
                }
                for (int i = int(nodes.size()) - 1; i >= 0; --i) {
                    int x = nodes[i];
                    if (parent[x] != -1) size[parent[x]] += size[x];
                }
                int rt = s;
                for (;;) {
                    int hv = -1;
                    for (int y : adj[rt])
                        if (!removed[y] && parent[y] == rt && size[y] > int(nodes.size() / 2)) {
                            hv = y;
                            break;
                        }
                    if (hv == -1) break;
                    rt = hv;
                }
                removed[rt] = true;
                dfsOrder.push_back(rt);
                // if (fa[s] != -1) cdt[fa[s]].push_back(rt);
                for (auto it = adj[rt].rbegin(); it != adj[rt].rend(); ++it)
                    if (!removed[*it]) {
                        q.push_back(*it);
                        // fa[*it] = rt;
                    }
            }
        }
    }
};
