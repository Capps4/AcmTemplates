#pragma once
#include "../../../../Headers/Headers.hpp"

inline int modSqrt(long long a, int p) {
    using i64 = long long;
    assert(p >= 2);
    a %= p;
    if (a < 0)
        a += p;
    if (a == 0 or p == 2)
        return int(a);
    auto pow = [&](i64 x, i64 k) {
        i64 res = 1;
        for (; k > 0; k >>= 1, x = x * x % p) {
            if (k & 1)
                res = res * x % p;
        }
        return res;
    };
    if (pow(a, (p - 1) / 2) != 1)
        return -1;
    i64 x;
    if (p % 4 == 3) {
        x = pow(a, (i64(p) + 1) / 4);
    } else {
        int r = 0;
        i64 q = p - 1, z = 2;
        while (q % 2 == 0) {
            q /= 2;
            ++r;
        }
        while (pow(z, (p - 1) / 2) != p - 1)
            ++z;
        i64 g = pow(z, q), b = pow(a, q);
        x = pow(a, (q + 1) / 2);
        while (b != 1) {
            int k = 0;
            for (i64 t = b; t != 1; t = t * t % p)
                ++k;
            g = pow(g, i64(1) << (r - k - 1));
            x = x * g % p;
            g = g * g % p;
            b = b * g % p;
            r = k;
        }
    }
    return int(std::min(x, p - x));
}
