#pragma once
#include "../../../../Headers/Headers.hpp"

class GeneralMatching {
    int n;
    std::vector<std::vector<int>> g;
    std::vector<int> pre{}, base{}, q{};
    std::vector<char> use{}, mark{}, vis{};

    int lca(int x, int y) {
        vis.assign(n, false);
        for (;;) {
            x = base[x];
            vis[x] = true;
            if (mate[x] == -1) {
                break;
            }
            x = pre[mate[x]];
        }
        while (!vis[base[y]]) {
            y = pre[mate[base[y]]];
        }
        return base[y];
    }

    void walk(int x, int b, int y) {
        while (base[x] != b) {
            mark[base[x]] = true;
            mark[base[mate[x]]] = true;
            pre[x] = y;
            y = mate[x];
            x = pre[y];
        }
    }

    bool aug(int s) {
        pre.assign(n, -1);
        base.resize(n);
        std::iota(base.begin(), base.end(), 0);
        use.assign(n, false);
        q.assign(1, s);
        use[s] = true;
        for (int i = 0; i < int(q.size()); ++i) {
            int x = q[i];
            for (int y : g[x]) {
                if (base[x] == base[y] or mate[x] == y) {
                    continue;
                }
                if (y == s or (mate[y] != -1 and pre[mate[y]] != -1)) {
                    int b = lca(x, y);
                    mark.assign(n, false);
                    walk(x, b, y);
                    walk(y, b, x);
                    for (int z = 0; z < n; ++z) {
                        if (mark[base[z]]) {
                            base[z] = b;
                            if (!use[z]) {
                                use[z] = true;
                                q.push_back(z);
                            }
                        }
                    }
                } else if (pre[y] == -1) {
                    pre[y] = x;
                    if (mate[y] == -1) {
                        while (y != -1) {
                            int z = pre[y], t = mate[z];
                            mate[y] = z;
                            mate[z] = y;
                            y = t;
                        }
                        return true;
                    }
                    int z = mate[y];
                    use[z] = true;
                    q.push_back(z);
                }
            }
        }
        return false;
    }

public:
    std::vector<int> mate;

    explicit GeneralMatching(int n) : n(n), g(n), mate(n, -1) {
        assert(n >= 0);
    }

    void add(int x, int y) {
        assert(0 <= x and x < n and 0 <= y and y < n);
        if (x != y) {
            g[x].push_back(y);
            g[y].push_back(x);
        }
    }

    int work() {
        int cnt = (n - int(std::count(mate.begin(), mate.end(), -1))) / 2;
        for (int x = 0; x < n; ++x) {
            if (mate[x] == -1 and aug(x)) {
                ++cnt;
            }
        }
        return cnt;
    }
};
