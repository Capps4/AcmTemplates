#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <int Z, char Base>
class Sam {
public:
    std::vector<std::array<int, Z>> son;
    std::vector<int> link, len;
    // std::vector<i64> cnt;
    int last = 0;
    unsigned int tot = 0;

    Sam(int n) : son(n * 2), link(n * 2), len(n * 2) {
        // cnt.assign(n * 2, 0);
        link[0] = -1;
    }

    Sam(const std::string &s) : Sam(s.size()) {
        for (auto i = 0U; i < s.size(); i++) {
            add(i, s[i]);
        }
    }

    void add(int id, char c) {
        c -= Base;
        int cur = ++tot;
        assert(tot < son.size());
        assign(id, cur, c);
        len[cur] = len[last] + 1;
        int v = last;
        while (v != -1 and !son[v][c]) {
            son[v][c] = cur;
            v = link[v];
        }

        if (v == -1) {
            link[cur] = 0;
        } else {
            int q = son[v][c];
            if (len[v] + 1 == len[q]) {
                link[cur] = q;
            } else {
                int clone = ++tot;
                assert(tot < son.size());
                len[clone] = len[v] + 1;
                son[clone] = son[q];
                link[clone] = link[q];

                while (v != -1 and son[v][c] == q) {
                    son[v][c] = clone;
                    v = link[v];
                }

                link[q] = link[cur] = clone;
            }
        }
        last = cur;
    }
private:
    void assign(int id, int cur, char c) {
    }
};

