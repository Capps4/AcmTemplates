#pragma once
#include "Include.hpp"

// Heap contraction/reconstruction: KACTL DirectedMST.h (CC0); see README.
template <class T>
class DirectedMST {
    static_assert(std::is_integral_v<T> and std::is_signed_v<T> and sizeof(T) <= 8);
    struct Edge {
        int x, y;
        T w;
    };
    struct Node {
        int l = -1, r = -1;
        __int128_t w = 0, tag = 0;
    };
    struct Heap {
        std::vector<Node> a{};
        std::vector<int> st{};

        void push(int x) {
            auto &v = a[x];
            v.w += v.tag;
            if (v.l != -1) {
                a[v.l].tag += v.tag;
            }
            if (v.r != -1) {
                a[v.r].tag += v.tag;
            }
            v.tag = 0;
        }

        int meld(int x, int y) {
            st.clear();
            while (x != -1 and y != -1) {
                push(x);
                push(y);
                if (a[x].w > a[y].w) {
                    std::swap(x, y);
                }
                st.push_back(x);
                x = a[x].r;
            }
            int z = x == -1 ? y : x;
            while (!st.empty()) {
                x = st.back();
                st.pop_back();
                a[x].r = z;
                std::swap(a[x].l, a[x].r);
                z = x;
            }
            return z;
        }

        int pop(int x) {
            push(x);
            return meld(a[x].l, a[x].r);
        }
    };
    struct Cycle {
        int x;
        std::size_t snap;
        std::vector<int> ids;
    };
    int n;
    std::vector<Edge> ed{};

public:
    struct Result {
        __int128_t cost = 0;
        std::vector<int> edges{};
    };

    explicit DirectedMST(int n) : n(n) {
        assert(n >= 0);
    }

    int add(int x, int y, T w) {
        assert(0 <= x and x < n and 0 <= y and y < n);
        assert(ed.size() < std::size_t(std::numeric_limits<int>::max()));
        int id = int(ed.size());
        ed.push_back({x, y, w});
        return id;
    }

    std::optional<Result> work(int root) const {
        assert(0 <= root and root < n);
        Heap h;
        h.a.resize(ed.size());
        std::vector<int> hp(n, -1), seen(n, -1), path(n), ids(n), in(n, -1);
        for (int id = 0; id < int(ed.size()); ++id) {
            h.a[id].w = ed[id].w;
            hp[ed[id].y] = h.meld(hp[ed[id].y], id);
        }
        RollbackDSU dsu(n);
        std::vector<Cycle> cyc;
        Result res;
        seen[root] = root;
        for (int s = 0; s < n; ++s) {
            int x = dsu.find(s), cnt = 0;
            while (seen[x] == -1) {
                while (hp[x] != -1 and dsu.find(ed[hp[x]].x) == x) {
                    hp[x] = h.pop(hp[x]);
                }
                if (hp[x] == -1) {
                    return std::nullopt;
                }
                int id = hp[x];
                h.push(id);
                __int128_t w = h.a[id].w;
                res.cost += w;
                h.a[id].tag -= w;
                hp[x] = h.pop(id);
                path[cnt] = x;
                ids[cnt++] = id;
                seen[x] = s;
                x = dsu.find(ed[id].x);
                if (seen[x] == s) {
                    int end = cnt, z = -1, hp0 = -1;
                    auto snap = dsu.snapshot();
                    do {
                        z = path[--cnt];
                        hp0 = h.meld(hp0, hp[z]);
                    } while (dsu.Union(x, z));
                    x = dsu.find(x);
                    hp[x] = hp0;
                    seen[x] = -1;
                    cyc.push_back({x, snap, {ids.begin() + cnt, ids.begin() + end}});
                }
            }
            for (int i = 0; i < cnt; ++i) {
                in[dsu.find(ed[ids[i]].y)] = ids[i];
            }
        }
        for (auto it = cyc.rbegin(); it != cyc.rend(); ++it) {
            int id = in[it->x];
            dsu.rollback(it->snap);
            for (int e : it->ids) {
                in[dsu.find(ed[e].y)] = e;
            }
            in[dsu.find(ed[id].y)] = id;
        }
        for (int x = 0; x < n; ++x) {
            if (x != root) {
                res.edges.push_back(in[x]);
            }
        }
        return res;
    }
};
