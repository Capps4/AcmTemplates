#pragma once
#include <cassert>
#include <cstddef>
#include <vector>

// SNIPPET BEGIN
class RingTree {
public:
    std::vector<std::vector<int>> g{}, rings{};
    // DSU dsu{}; // 如需合并挂树，先引入 DSU 并启用相关注释。

    explicit RingTree(const std::vector<std::vector<int>> &adj) : g(adj.size()) {
        int n = int(adj.size());
        // dsu.init(n);
        std::vector<int> deg(n);
        std::vector<char> vis(n);
        std::vector<int> queue;
        queue.reserve(n);
        for (int x = 0; x < n; ++x) {
            deg[x] = adj[x].size();
            if (deg[x] <= 1) queue.push_back(x);
        }
        for (int i = 0; i < int(queue.size()); ++i) {
            int x = queue[i];
            vis[x] = true;
            for (int y : adj[x]) {
                assert(0 <= y && y < n);
                if (vis[y]) continue;
                // dsu.Union(y, x);
                g[y].push_back(x);
                if (--deg[y] == 1) queue.push_back(y);
            }
            deg[x] = 0;
        }
        for (int x = 0; x < n; ++x)
            assert(deg[x] == 0 || deg[x] == 2);
        for (int s = 0; s < n; ++s) {
            if (vis[s]) continue;
            rings.emplace_back();
            int x = s;
            while (x != -1) {
                rings.back().push_back(x);
                vis[x] = true;
                int z = -1;
                for (int y : adj[x]) {
                    assert(0 <= y && y < n);
                    if (!vis[y]) {
                        z = y;
                        break;
                    }
                }
                x = z;
            }
        }
    }

    explicit RingTree(const std::vector<int> &link) : g(link.size()) {
        int n = int(link.size());
        // dsu.init(n);
        std::vector<int> deg(n);
        std::vector<int> queue;
        queue.reserve(n);
        for (int y : link) {
            assert(0 <= y && y < n);
            ++deg[y];
        }
        for (int x = 0; x < n; ++x)
            if (!deg[x]) queue.push_back(x);
        for (int i = 0; i < int(queue.size()); ++i) {
            int x = queue[i], y = link[x];
            // dsu.Union(y, x);
            g[y].push_back(x);
            if (--deg[y] == 0) queue.push_back(y);
        }
        for (int s = 0; s < n; ++s) {
            if (!deg[s]) continue;
            rings.emplace_back();
            int x = s;
            do {
                rings.back().push_back(x);
                deg[x] = 0;
                x = link[x];
            } while (x != s);
        }
    }
};
