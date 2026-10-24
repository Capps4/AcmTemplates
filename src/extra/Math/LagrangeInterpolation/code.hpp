#pragma once
#include "Include.hpp"

template <class T>
T lagrangeInterpolation(const std::vector<T> &y, T x) {
    assert(y.size() < std::size_t(INT_MAX));
    int n = y.size();
    assert(n <= T::getMod());
    if (n == 0)
        return T(0);
    if (x.val() < n)
        return y[int(x.val())];
    auto &cb = Comb<T>::shared(n - 1);
    std::vector<T> pre(n + 1, T(1));
    for (int i = 0; i < n; ++i)
        pre[i + 1] = pre[i] * (x - T(i));
    T suf = 1, res = 0;
    for (int i = n - 1; i >= 0; --i) {
        T val = y[i] * pre[i] * suf * cb.ijc(i) * cb.ijc(n - i - 1);
        res += (n - i - 1) % 2 ? -val : val;
        suf *= x - T(i);
    }
    return res;
}
