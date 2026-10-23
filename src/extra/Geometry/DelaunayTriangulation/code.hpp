#pragma once
#include "Include.hpp"

namespace _delaunay {
using ld = long double;
using P = Point<ld>;
using PS = std::vector<P>;
inline const ld pi = std::acos(-1.L);
inline int sgn(ld x) { return (x > 0) - (x < 0); }
// a-b 和 a-c 的夹角 signed
inline ld ang(P a, P b, P c) {
    auto v1 = a.to(b), v2 = a.to(c);
    return std::atan2(v1.cross(v2), v1.dot(v2)); // ∠bac
}

// 对四个不同的点判断四点共圆
// d在abc外接圆外return 1, 内return -1
inline int inc(P a, P b, P c, P d) {
    auto ag1 = ang(a, b, c), ag2 = ang(d, c, b);
    if (sgn(ag1) == sgn(ag2)) {
        return sgn(pi - std::abs(ag1 + ag2));
    } else {
        return sgn(std::abs(ag1) - std::abs(ag2));
    }
}

struct Delaunay {
    struct Edge {
        Edge *l, *r, *inv;
        int face, from;

        Edge() : l(nullptr), r(nullptr), inv(nullptr), face(0), from(-1) {}
        Edge(const Edge&) = delete;
        Edge& operator=(const Edge&) = delete;
    };

    void join(Edge *e0, Edge *e1, Edge *e2, int fid) {
        e0->r = e1;
        e2->l = e1;
        e1->r = e2;
        e0->l = e2;
        e2->r = e0;
        e1->l = e0;
        e0->face = fid;
        e1->face = fid;
        e2->face = fid;
        face[fid] = e0;
    }

    void init() {
        bag.resize(2 * (n + 1));
        vertex.resize(n + 3);

        // 加三个点构造全局三角形
        ps.emplace_back(Inf * 10, Inf);
        ps.emplace_back(-Inf, Inf);
        ps.emplace_back(-Inf, -Inf * 10);

        std::vector<Edge *> e(6);
        for (auto &pt : e) {
            pool.emplace_back();
            pt = &pool.back();
        }

        e[0]->from = n;
        e[5]->from = n;
        e[1]->from = n + 1;
        e[3]->from = n + 1;
        e[2]->from = n + 2;
        e[4]->from = n + 2;

        for (int i = 0; i < 3; i++) {
            e[i]->inv = e[i + 3];
            e[i + 3]->inv = e[i];
        }

        face.resize(2);
        join(e[0], e[1], e[2], 0);
        join(e[5], e[4], e[3], 1);

        for (int i = 0; i < 3; i++) {
            vertex[i + n] = e[i];
        }

        bag[1].resize(n);
        std::iota(begin(bag[1]), end(bag[1]), 0);
        bel.assign(n, 1);
    }

    std::vector<Edge *> vertex, face;
    std::vector<std::vector<int>> bag;
    std::vector<int> bel;
    PS ps;
    const int n;
    std::deque<Edge> pool{};

    Delaunay(const Delaunay&) = delete;
    Delaunay& operator=(const Delaunay&) = delete;

    static constexpr int Inf = int(1e8);
    explicit Delaunay(const PS &pts)
        : vertex(), face(), bag(), bel(), ps(pts), n(int(pts.size())) {
        for (const auto& p : ps) {
            assert(std::isfinite(p.x) and std::isfinite(p.y));
            assert(std::abs(p.x) < Inf and std::abs(p.y) < Inf);
        }
        if (n < 3) {
            return;
        }
        init();

        for (int i = 0; i < n; i++) {
            put(i);

            Edge *e = vertex[i];
            for (int j = 0; j < 3; j++) {
                if (flip(i, e->r->inv)) {
                    e = e->l->inv;
                    j -= 2;
                } else {
                    e = e->inv->r;
                }
            }
        }
    }

    bool has(P p, int fid) {
        int cnt = 0;
        Edge *e = face[fid];
        do {
            auto a = ps[e->from];
            e = e->r;
            auto b = ps[e->from];

            cnt += a.to(b).cross(a.to(p)) > 0;
        } while (e != face[fid]);

        return cnt == 3 or cnt == 0;
    }

    void put(int pid) {
        int fid[3] = {bel[pid], int(face.size()), int(face.size()) + 1};

        auto pts = bag[fid[0]];
        bag[fid[0]].clear();

        face.resize(face.size() + 2);

        std::vector<Edge *> e(6);
        for (int i = 0; i < 6; i++) {
            pool.emplace_back();
            e[i] = &pool.back();
        }
        for (int i = 0; i < 6; i++) {
            e[i]->inv = e[i ^ 1];
        }

        e[1]->from = pid;
        e[3]->from = pid;
        e[5]->from = pid;
        Edge *tmpe = face[fid[0]];
        int r = 0, l = 5;
        for (int j = 0; j < 3; j++) {
            Edge *tmpr = tmpe->r;

            int b = tmpr->from;
            e[r]->from = b;

            join(e[l], tmpe, e[r], fid[j]);

            r = (r + 2) % 6;
            l = (l + 2) % 6;
            tmpe = tmpr;
        }

        vertex[pid] = e[1];

        for (int p : pts) {
            if (p == pid) {
                continue;
            }
            bool hit = false;
            for (int j = 0; j < 3; j++) {
                if (has(ps[p], fid[j])) {
                    hit = true;
                    bel[p] = fid[j];
                    bag[fid[j]].push_back(p);
                    break;
                }
            }
            if (!hit) {
                throw std::domain_error("Delaunay: degenerate or numerically unstable point");
            }
        }
    }

    bool flip(int pid, Edge *e) {
        if (e->face == 0) {
            return false;
        }

        auto fir = e->inv->l;

        int res = inc(ps[pid], ps[fir->r->from],
                                   ps[fir->l->from], ps[e->l->from]);

        if (res >= 0) {
            return false;
        }

        int fid[2] = {fir->face, e->face};

        auto pts = bag[fid[0]];
        pts.insert(pts.end(), bag[fid[1]].begin(), bag[fid[1]].end());

        bag[fid[0]].clear();
        bag[fid[1]].clear();

        auto ev = e->inv;
        auto sec = e->r, thir = e->l, four = fir->l;

        join(fir, sec, ev, fid[0]);
        join(e, thir, four, fid[1]);
        e->from = pid;
        ev->from = thir->from;

        for (int p : pts) {
            bool hit = false;
            for (int j = 0; j < 2; j++) {
                if (has(ps[p], fid[j])) {
                    hit = true;
                    bel[p] = fid[j];
                    bag[fid[j]].push_back(p);
                    break;
                }
            }
            if (!hit) {
                throw std::domain_error("Delaunay: unstable edge flip");
            }
        }
        return true;
    }
};
} // namespace _delaunay

using Delaunay = _delaunay::Delaunay;
