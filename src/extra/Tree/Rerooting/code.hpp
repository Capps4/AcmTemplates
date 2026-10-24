#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
template <class T>
class Rerooting {
public:
    std::vector<T> ans;

    template <class M, class L, class F>
    Rerooting(const std::vector<std::vector<int>> &adj, const T &e, M op, L lift, F put)
        : ans(adj.size(), e) {
        int n = int(adj.size());
        std::vector<int> fa(n, -2), q;
        q.reserve(n);
        for (int rt = 0; rt < n; ++rt) {
            if (fa[rt] != -2)
                continue;
            fa[rt] = -1;
            q.push_back(rt);
            for (int i = int(q.size()) - 1; i < int(q.size()); ++i) {
                int x = q[i];
                for (int y : adj[x]) {
                    assert(0 <= y and y < n);
                    if (y == fa[x])
                        continue;
                    assert(fa[y] == -2); // Input must be an undirected forest.
                    fa[y] = x;
                    q.push_back(y);
                }
            }
        }
        std::vector<T> down(n, e), up(n, e), pre;
        for (int i = n - 1; i >= 0; --i) {
            int x = q[i];
            T val = e;
            for (int y : adj[x])
                if (y != fa[x])
                    val = op(val, lift(down[y], y, x));
            down[x] = put(val, x);
        }
        for (int x : q) {
            int d = int(adj[x].size());
            pre.assign(1, e);
            for (int y : adj[x])
                pre.push_back(op(pre.back(), y == fa[x] ? up[x] : lift(down[y], y, x)));
            ans[x] = put(pre.back(), x);
            T suf = e;
            for (int i = d - 1; i >= 0; --i) {
                int y = adj[x][i];
                if (y != fa[x])
                    up[y] = lift(put(op(pre[i], suf), x), x, y);
                T val = y == fa[x] ? up[x] : lift(down[y], y, x);
                suf = op(val, suf);
            }
        }
    }
};
