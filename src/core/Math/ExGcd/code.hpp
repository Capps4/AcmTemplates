#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
// Nonnegative a,b; coefficients and intermediates must fit T.
template <class T>
constexpr T exgcd(T a, T b, T &x, T &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    T g = exgcd(b, T(a % b), y, x);
    y -= a / b * x;
    return g;
}
