#pragma once
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

// SNIPPET BEGIN
template <class T, class Cmp = std::greater<T>>
struct Max {
    Cmp cmp;

    explicit Max(Cmp cmp = {}) : cmp(std::move(cmp)) {}

    constexpr T operator()(const T &a, const T &b) const { return cmp(b, a) ? b : a; }
};

// Commutative associative merge with a two-sided identity.
template <class T, class Merge = std::plus<T>>
class Fenwick {
    const int n;
    int firstStep = 1;
    std::vector<T> t;
    Merge merge;
    const T identity;

public:
    explicit Fenwick(int n, Merge merge = {}, T identity = {})
        : n(n), t(n + 1, identity), merge(std::move(merge)),
          identity(std::move(identity)) {
        assert(n >= 0);
        while (firstStep <= n / 2)
            firstStep *= 2;
    }

    explicit Fenwick(const std::vector<T> &a, Merge merge = {}, T identity = {})
        : Fenwick(int(a.size()), std::move(merge), std::move(identity)) {
        for (int i = 1; i <= n; ++i) {
            t[i] = this->merge(t[i], a[i - 1]);
            int fa = i + (i & -i);
            if (fa <= n) t[fa] = this->merge(t[fa], t[i]);
        }
    }

    void modify(int i, const T &v) {
        assert(0 <= i && i < n);
        for (++i; i <= n; i += i & -i)
            t[i] = merge(t[i], v);
    }

    T posQuery(int i) const {
        T res = identity;
        for (++i; i > 0; i -= i & -i)
            res = merge(res, t[i]);
        return res;
    }

    // [l, r), additive merge.
    T rangeQuery(int l, int r) const { return posQuery(r - 1) - posQuery(l - 1); }

    // Inclusive endpoint of the longest prefix <= lim; empty is -1.
    // Prefixes must be monotone.
    int select(const T &lim) const {
        int r = 0;
        T cur = identity;
        for (int step = firstStep; step; step /= 2) {
            if (step > n - r) continue;
            T y = merge(cur, t[r + step]);
            if (y <= lim) {
                r += step;
                cur = std::move(y);
            }
        }
        return r - 1;
    }
};
