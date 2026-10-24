#pragma once
#include "../../../../Headers/Headers.hpp"

enum class BitwiseOp { Xor, And, Or };

template <BitwiseOp Op, class T>
void bitwiseTransform(std::vector<T> &a, bool inv = false) {
    std::size_t n = a.size();
    assert(n == 0 or (n & (n - 1)) == 0);
    for (std::size_t k = 1; k < n; k *= 2)
        for (std::size_t i = 0; i < n; i += 2 * k)
            for (std::size_t j = 0; j < k; ++j) {
                T &u = a[i + j], &v = a[i + j + k];
                if constexpr (Op == BitwiseOp::Xor) {
                    T x = u;
                    u += v;
                    v = x - v;
                } else if constexpr (Op == BitwiseOp::And) {
                    u = inv ? u - v : u + v;
                } else {
                    v = inv ? v - u : v + u;
                }
            }
    if constexpr (Op == BitwiseOp::Xor) {
        if (inv and n != 0) {
            if constexpr (std::is_integral_v<T>) {
                for (auto &x : a)
                    x /= T(n);
            } else {
                T iv = T(1) / T(n);
                for (auto &x : a)
                    x *= iv;
            }
        }
    }
}

template <BitwiseOp Op, class T>
std::vector<T> bitwiseConvolution(std::vector<T> a, std::vector<T> b) {
    assert(a.size() == b.size());
    bitwiseTransform<Op>(a);
    bitwiseTransform<Op>(b);
    for (std::size_t i = 0; i < a.size(); ++i)
        a[i] *= b[i];
    bitwiseTransform<Op>(a, true);
    return a;
}
