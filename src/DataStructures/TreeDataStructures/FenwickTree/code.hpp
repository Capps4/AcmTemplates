#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
template <class T, class Cmp = std::greater<T>>
struct Max {
    Cmp cmp;

    explicit Max(Cmp cmp = {}) : cmp(std::move(cmp)) {}

    constexpr T operator()(const T &a, const T &b) const {
        return cmp(b, a) ? b : a;
    }
};

// Commutative associative merge with a two-sided identity.
template <class T, class Merge = std::plus<T>>
class Fenwick {
    const int n;
    std::vector<T> t;
    Merge op;
    const T unit;

public:
    explicit Fenwick(int n, Merge op = {}, T unit = {})
        : n(n), t(n + 1, unit), op(std::move(op)), unit(std::move(unit)) {
        assert(n >= 0);
    }

    explicit Fenwick(const std::vector<T> &a, Merge op = {}, T unit = {})
        : Fenwick(int(a.size()), std::move(op), std::move(unit)) {
        for (int i = 1; i <= n; ++i) {
            t[i] = this->op(t[i], a[i - 1]);
            int fa = i + (i & -i);
            if (fa <= n)
                t[fa] = this->op(t[fa], t[i]);
        }
    }

    void modify(int i, const T &v) {
        assert(0 <= i and i < n);
        for (++i; i <= n; i += i & -i)
            t[i] = op(t[i], v);
    }

    T query(int i) const {
        T res = unit;
        for (++i; i > 0; i -= i & -i)
            res = op(res, t[i]);
        return res;
    }

    // Inclusive endpoint of the longest prefix <= lim; empty is -1.
    // Prefixes must be monotone.
    int select(const T &lim) const {
        int r = 0;
        T cur = unit;
#ifdef _LIBCPP_VERSION
        int step = n ? 1 << (31 - __builtin_clz(unsigned(n))) : 0;
#else
        int step = n ? 1 << std::__lg(n) : 0;
#endif
        for (; step; step /= 2) {
            if (step > n - r)
                continue;
            T y = op(cur, t[r + step]);
            if (y <= lim) {
                r += step;
                cur = std::move(y);
            }
        }
        return r - 1;
    }
};
