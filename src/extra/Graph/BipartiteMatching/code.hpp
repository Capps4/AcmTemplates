#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
class BipartiteMatching {
    int n, m;
    std::vector<std::vector<int>> g;
    std::vector<int> dep{}, cur{};

    int bfs() {
        dep.assign(n, -1);
        std::vector<int> q;
        q.reserve(n);
        for (int x = 0; x < n; ++x)
            if (matchL[x] == -1) {
                dep[x] = 0;
                q.push_back(x);
            }
        int lim = n + 1;
        for (int i = 0; i < int(q.size()); ++i) {
            int x = q[i];
            if (dep[x] >= lim)
                continue;
            for (int y : g[x]) {
                int z = matchR[y];
                if (z == -1)
                    lim = dep[x] + 1;
                else if (dep[z] == -1) {
                    dep[z] = dep[x] + 1;
                    q.push_back(z);
                }
            }
        }
        return lim;
    }

    bool dfs(int s, int lim, std::vector<int> &st) {
        st.assign(1, s);
        while (!st.empty()) {
            int x = st.back();
            bool go = false;
            while (cur[x] < int(g[x].size())) {
                int y = g[x][cur[x]++], z = matchR[y];
                if (z == -1 and dep[x] + 1 == lim) {
                    for (int i = int(st.size()) - 1; i >= 0; --i) {
                        x = st[i];
                        std::swap(y, matchL[x]);
                        matchR[matchL[x]] = x;
                    }
                    return true;
                }
                if (z != -1 and dep[z] == dep[x] + 1 and dep[z] < lim) {
                    st.push_back(z);
                    go = true;
                    break;
                }
            }
            if (!go) {
                dep[x] = -1;
                st.pop_back();
            }
        }
        return false;
    }

public:
    std::vector<int> matchL, matchR;

    BipartiteMatching(int n, int m) : n(n), m(m), g(n), matchL(n, -1), matchR(m, -1) {
        assert(0 <= n and n < std::numeric_limits<int>::max() and 0 <= m);
    }

    void add(int x, int y) {
        assert(0 <= x and x < n and 0 <= y and y < m);
        g[x].push_back(y);
    }

    int work() {
        int cnt = n - int(std::count(matchL.begin(), matchL.end(), -1));
        std::vector<int> st;
        st.reserve(n);
        for (;;) {
            int lim = bfs();
            if (lim == n + 1)
                return cnt;
            cur.assign(n, 0);
            for (int x = 0; x < n; ++x)
                if (matchL[x] == -1 and dfs(x, lim, st))
                    ++cnt;
        }
    }

    std::pair<std::vector<int>, std::vector<int>> minCover() {
        work();
        std::vector<char> a(n), b(m);
        std::vector<int> q;
        for (int x = 0; x < n; ++x)
            if (matchL[x] == -1) {
                a[x] = true;
                q.push_back(x);
            }
        for (int i = 0; i < int(q.size()); ++i)
            for (int y : g[q[i]])
                if (y != matchL[q[i]] and !b[y]) {
                    b[y] = true;
                    int x = matchR[y];
                    if (x != -1 and !a[x]) {
                        a[x] = true;
                        q.push_back(x);
                    }
                }
        std::pair<std::vector<int>, std::vector<int>> res;
        for (int x = 0; x < n; ++x)
            if (!a[x])
                res.first.push_back(x);
        for (int y = 0; y < m; ++y)
            if (b[y])
                res.second.push_back(y);
        return res;
    }
};
