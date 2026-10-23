#pragma once
#include "Include.hpp"

template <class T>
struct RangeBIT {
    const int n;
    Fenwick<T> d, s;

    explicit RangeBIT(int n) : n(n), d(n), s(n) {}

    explicit RangeBIT(const std::vector<T>& a)
        : n(int(a.size())), d(diff(a)), s(multIndex(diff(a))) {}

    static std::vector<T> diff(std::vector<T> a) {
        std::adjacent_difference(a.begin(), a.end(), a.begin());
        return a;
    }

    static std::vector<T> multIndex(std::vector<T> a) {
        for (int i = 0; i < int(a.size()); ++i) {
            a[i] = a[i] * i;
        }
        return a;
    }

    void rangeModify(int l, int r, const T& x) {
        assert(0 <= l and l <= r and r <= n);
        if (l == r) {
            return;
        }
        d.modify(l, x);
        s.modify(l, x * l);
        if (r < n) {
            d.modify(r, T{} - x);
            s.modify(r, T{} - x * r);
        }
    }

    T rangeQuery(int l, int r) const {
        assert(0 <= l and l <= r and r <= n);
        return d.query(r - 1) * r - s.query(r - 1) - d.query(l - 1) * l + s.query(l - 1);
    }
};
