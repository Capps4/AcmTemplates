#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
/*
维基百科 :
n < 4e9, primes = [2, 7, 61]
n < 3e14, primes = [2, 3, 5, 7, 11, 13, 17]
n < 3e18, primes = [2, 3, 5, 7, 11, 13, 17, 19, 23]
n < 3e23, primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37]
*/
// 上面是原版基底范围注释；当前 64 位分支使用下面七个确定性基底覆盖完整范围。
template <class T>
class MillerRabin {
    static_assert(std::is_integral_v<T> and !std::is_same_v<T, bool> and sizeof(T) <= 8);
    using U = std::make_unsigned_t<T>;

    static constexpr U mul(U a, U b, U mod) {
        if constexpr (sizeof(T) <= 4)
            return U(std::uint64_t(a) * b % mod);
        else
            return U(static_cast<unsigned __int128>(a) * b % mod);
    }

    static constexpr U pow(U base, U exp, U mod) {
        U res = 1;
        for (; exp; exp >>= 1) {
            if (exp & 1)
                res = mul(res, base, mod);
            base = mul(base, base, mod);
        }
        return res;
    }

public:
    // Deterministic throughout the input type's complete 32/64-bit range.
    constexpr bool operator()(T v) const {
        if (v < 2)
            return false;
        U n = U(v);
        for (U p : {U(2), U(3), U(5), U(7)}) {
            if (n % p == 0)
                return n == p;
        }
        U odd = n - 1;
        int twos = 0;
        while (!(odd & 1)) {
            odd >>= 1;
            ++twos;
        }
        auto test = [&](U base) constexpr {
            base %= n;
            if (base == 0)
                return true;
            U x = pow(base, odd, n);
            if (x == 1 or x == n - 1)
                return true;
            for (int i = 1; i < twos; ++i) {
                x = mul(x, x, n);
                if (x == n - 1)
                    return true;
            }
            return false;
        };
        if constexpr (sizeof(T) <= 4) {
            for (U base : {U(2), U(7), U(61)})
                if (!test(base))
                    return false;
        } else {
            for (U base : {U(2), U(325), U(9375), U(28178), U(450775), U(9780504), U(1795265022)})
                if (!test(base))
                    return false;
        }
        return true;
    }
};

constexpr MillerRabin<long long> isPrime;
