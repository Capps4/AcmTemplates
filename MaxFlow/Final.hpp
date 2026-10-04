#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
template <class T>
class Flow {
    static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>);

    std::vector<int> cur{}, dep{}, queue{};

    bool bfs(int s, int t) {
        dep.assign(n, -1);
        queue.clear();
        queue.reserve(n);
        queue.push_back(s);
        dep[s] = 0;
        for (int i = 0; i < int(queue.size()); ++i) {
            int x = queue[i];
            for (int id : g[x]) {
                const auto &[y, cap] = adj[id];
                if (cap <= 0 || dep[y] != -1) continue;
                dep[y] = dep[x] + 1;
                if (y == t) return true;
                queue.push_back(y);
            }
        }
        return false;
    }

    T dfs(int u, int t, T f) {
        if (u == t) return f;
        T res = 0;
        for (int &i = cur[u]; i < int(g[u].size()); ++i) {
            int j = g[u][i];
            const auto &[v, c] = adj[j];
            if (c > 0 && dep[v] == dep[u] + 1) {
                T out = dfs(v, t, std::min(f, c));
                adj[j].second -= out;
                adj[j ^ 1].second += out;
                res += out;
                f -= out;
                if (f == 0) return res;
            }
        }
        return res;
    }

public:
    static constexpr T Inf = std::numeric_limits<T>::max();
    int n;
    std::vector<std::pair<int, T>> adj{};
    std::vector<std::vector<int>> g;

    explicit Flow(int size) : n(size), g(n) {}

    int newNode() {
        g.emplace_back();
        return n++;
    }

    int add(int s, int t, T cap, T rev = 0) {
        assert(0 <= s && s < n && 0 <= t && t < n);
        assert(cap >= 0 && rev >= 0 && cap <= Inf - rev);
        if constexpr (std::is_floating_point_v<T>) assert(std::isfinite(cap) && std::isfinite(rev));
        int id = int(adj.size());
        g[s].push_back(id);
        adj.emplace_back(t, cap);
        g[t].push_back(id + 1);
        adj.emplace_back(s, rev);
        return id;
    }

    // Returns additional flow, modifying residual capacities. Call again to continue.
    T work(int s, int t, T limit = Inf) {
        assert(0 <= s && s < n && 0 <= t && t < n && limit >= 0);
        if (s == t || limit == 0) return 0;
        T res = 0;
        while (res < limit && bfs(s, t)) {
            cur.assign(n, 0);
            res += dfs(s, t, limit - res);
        }
        return res;
    }

    std::string getReach(int s) const {
        assert(0 <= s && s < n);
        std::string vis(n, 't');
        std::vector<int> q{s};
        vis[s] = 's';
        for (int i = 0; i < int(q.size()); ++i)
            for (int id : g[q[i]]) {
                const auto &[y, cap] = adj[id];
                if (cap > 0 && vis[y] == 't') {
                    vis[y] = 's';
                    q.push_back(y);
                }
            }
        return vis;
    }

    std::vector<std::pair<int, int>> getCuts(int s) const {
        auto vis = getReach(s);
        std::vector<std::pair<int, int>> res;
        for (int x = 0; x < n; ++x)
            if (vis[x] == 's')
                for (int id : g[x])
                    if (vis[adj[id].first] == 't') res.emplace_back(x, adj[id].first);
        return res;
    }
};
