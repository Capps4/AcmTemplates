#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T>
T linearRecurrence(const std::vector<T> &a, const std::vector<T> &c,
                   std::uint64_t k) {
    assert(c.size() <= std::size_t(std::numeric_limits<int>::max() / 2));
    assert(a.size() >= c.size());
    int n = int(c.size());
    if (k < a.size())
        return a[std::size_t(k)];
    if (n == 0)
        return T{};
    auto mul = [&](const std::vector<T> &x, const std::vector<T> &y) {
        std::vector<T> z(2 * std::size_t(n) - 1);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                z[i + j] += x[i] * y[j];
        for (int i = 2 * n - 2; i >= n; --i)
            for (int j = 0; j < n; ++j)
                z[i - j - 1] += z[i] * c[j];
        z.resize(n);
        return z;
    };
    std::vector<T> res(n), x(n);
    res[0] = T(1);
    x[n == 1 ? 0 : 1] = n == 1 ? c[0] : T(1);
    while (k != 0) {
        if (k & 1)
            res = mul(res, x);
        k >>= 1;
        if (k != 0)
            x = mul(x, x);
    }
    T ans{};
    for (int i = 0; i < n; ++i)
        ans += res[i] * a[i];
    return ans;
}

template <class T>
std::vector<T> berlekampMassey(const std::vector<T> &a) {
    static_assert(!std::is_integral_v<T>, "Berlekamp-Massey needs a field");
    assert(a.size() <= std::size_t(std::numeric_limits<int>::max() / 2));
    std::vector<T> c{T(1)}, b{T(1)};
    int n = 0, m = 1;
    T d = T(1);
    for (int i = 0; i < int(a.size()); ++i) {
        T v = a[i];
        for (int j = 1; j <= n; ++j)
            v += c[j] * a[i - j];
        if (v == T{}) {
            ++m;
            continue;
        }
        auto pre = c;
        T q = v / d;
        c.resize(std::max(c.size(), b.size() + m));
        for (int j = 0; j < int(b.size()); ++j)
            c[j + m] -= q * b[j];
        if (2 * n <= i) {
            n = i + 1 - n;
            b = std::move(pre);
            d = v;
            m = 1;
        } else {
            ++m;
        }
    }
    c.resize(n + 1);
    c.erase(c.begin());
    for (auto &x : c)
        x = -x;
    return c;
}
