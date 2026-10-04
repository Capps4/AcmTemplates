#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <class T>
class Flow {
    std::vector<int> cur{}, dep{};
    bool bfs(int s, int t) {
        dep.assign(n, -1);
        std::queue<int> q;
        dep[s] = 0;

        q.push(s);
        while (!q.empty()) {
            const int u = q.front();
            q.pop();

            for (int i : g[u]) {
                auto pr = adj[i];
                int v = pr.first;
                T c = pr.second;

                if (c > 0 and dep[v] == -1) {
                    dep[v] = dep[u] + 1;
                    if (v == t)
                        return true;
                    q.push(v);
                }
            }
        }

        return false;
    }

    T dfs(int u, int t, T f) {
        if (u == t) {
            return f;
        }
        T res = f;
        for (int &i = cur[u]; i < static_cast<int>(g[u].size()); i++) {
            const int j = g[u][i];
            auto pr = adj[j];
            int v = pr.first;
            T c = pr.second;

            if (c > 0 and dep[v] == dep[u] + 1) {
                T out = dfs(v, t, std::min(res, c));
                adj[j].second -= out;
                adj[j ^ 1].second += out;

                res -= out;
                if (res == 0) {
                    return f;
                }
            }
        }
        return f - res;
    }

public:
    int n;
    std::vector<std::pair<int, T>> adj{};
    std::vector<std::vector<int>> g;
    static constexpr T Inf = std::numeric_limits<T>::max();

    Flow(int m) : n(m), g(m) {}

    int newNode() {
        n += 1;
        g.push_back({});
        return n - 1;
    }

    int add(int u, int v, T c, T c_ = 0) {
        int eid = adj.size();
        g[u].push_back(adj.size());
        adj.emplace_back(v, c);
        g[v].push_back(adj.size());
        adj.emplace_back(u, c_);

        return eid;
    }

    T work(int s, int t) {
        T ans = 0;
        while (bfs(s, t)) {
            cur.assign(n, 0);
            ans += dfs(s, t, Inf);
        }
        return ans;
    }

    std::string getReach(int s) {
        std::string vis(n, 't');
        vis[s] = 's';

        std::queue<int> q;
        q.push(s);

        while (!q.empty()) {
            int x = q.front();
            q.pop();
            for (int i : g[x]) {
                auto pr = adj[i];
                int y = pr.first;
                T w = pr.second;
                if (vis[y] == 's' or w == 0) {
                    continue;
                }
                vis[y] = 's';
                q.push(y);
            }
        }

        return vis;
    }

    std::vector<std::pair<int, int>> getCuts(int s) {
        std::string vis = getReach(s);
        std::vector<std::pair<int, int>> cuts;
        for (int x = 0; x < n; x++) {
            if (vis[x] == 't') {
                continue;
            }

            for (int i : g[x]) {
                auto pr = adj[i];
                int y = pr.first;
                T w = pr.second;
                if (vis[y] == 't') {
                    assert(w == 0);
                    cuts.emplace_back(x, y);
                }
            }
        }

        return cuts;
    }
};

