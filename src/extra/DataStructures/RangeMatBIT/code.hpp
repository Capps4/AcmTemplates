#pragma once
#include "Include.hpp"

template <class T>
struct RangeMatBIT {
    MatBIT<T> p, px, py, pxy;

    RangeMatBIT(int n, int m) : p(n, m), px(n, m), py(n, m), pxy(n, m) {}

    void rangeModify(int u, int l, int d, int r, const T& v) {
        assert(0 <= u and u <= d and d <= p.n and 0 <= l and l <= r and r <= p.m);
        if (u == d or l == r) {
            return;
        }
        modify(u, l, v);
        modify(d, r, v);
        modify(u, r, T{} - v);
        modify(d, l, T{} - v);
    }

    T rangeQuery(int u, int l, int d, int r) const {
        assert(0 <= u and u <= d and d <= p.n and 0 <= l and l <= r and r <= p.m);
        return query(d, r) + query(u, l) - query(d, l) - query(u, r);
    }

private:
    void modify(int x, int y, const T& v) {
        if (x == p.n or y == p.m) {
            return;
        }
        p.modify(x, y, v);
        px.modify(x, y, v * x);
        py.modify(x, y, v * y);
        pxy.modify(x, y, v * x * y);
    }

    T query(int x, int y) const {
        return p.posQuery(x - 1, y - 1) * x * y - px.posQuery(x - 1, y - 1) * y
             - py.posQuery(x - 1, y - 1) * x + pxy.posQuery(x - 1, y - 1);
    }
};
