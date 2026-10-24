#pragma once
#include "../../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
template <class T, class F = const T& (*)(const T&, const T&)>
class RMQ {
    const int n;
    F f;
    std::vector<std::vector<T>> jump{};

    static int lg(unsigned x) {
        return 31 - __builtin_clz(x);
    }

public:
    explicit RMQ(const std::vector<T> &a, F f) : n(a.size()), f(std::move(f)) {
        if (!n)
            return;
        int logn = lg(n);
        jump.resize(logn + 1);
        jump[0] = a;
        for (int j = 1; j <= logn; ++j) {
            int half = 1 << (j - 1), cnt = n - (1 << j) + 1;
            jump[j].resize(cnt);
            for (int i = 0; i < cnt; ++i)
                jump[j][i] = this->f(jump[j - 1][i], jump[j - 1][i + half]);
        }
    }

    // [l,r); f is associative and idempotent, not necessarily commutative.
    T operator()(int l, int r) const {
        assert(0 <= l and l < r and r <= n);
        int log = lg(r - l);
        return f(jump[log][l], jump[log][r - (1 << log)]);
    }
};
