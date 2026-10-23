#pragma once
#include "Include.hpp"

template <class T, class Merge = std::plus<T>>
struct MatBIT {
    const int n, m;
    Merge merge;
    std::vector<Fenwick<T, Merge>> t;

    MatBIT(int n, int m, Merge op = {})
        : n(n), m(m), merge(std::move(op)), t(n + 1, Fenwick<T, Merge>(m, merge)) {
        assert(n >= 0 and m >= 0);
    }

    void modify(int x, int y, const T& v) {
        assert(0 <= x and x < n and 0 <= y and y < m);
        for (int i = x + 1; i <= n; i += i & -i) {
            t[i].modify(y, v);
        }
    }

    T posQuery(int x, int y) const {
        assert(-1 <= x and x < n and -1 <= y and y < m);
        T ans{};
        for (int i = x + 1; i > 0; i -= i & -i) {
            ans = merge(ans, t[i].query(y));
        }
        return ans;
    }

    T rangeQuery(int u, int l, int d, int r) const {
        assert(0 <= u and u <= d and d <= n and 0 <= l and l <= r and r <= m);
        return posQuery(d - 1, r - 1) + posQuery(u - 1, l - 1)
             - posQuery(d - 1, l - 1) - posQuery(u - 1, r - 1);
    }
};
