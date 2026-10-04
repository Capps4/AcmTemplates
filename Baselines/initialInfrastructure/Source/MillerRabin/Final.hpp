#pragma once
#include <cstdint>
#include <initializer_list>
#include <type_traits>

// SNIPPET BEGIN
template <class T>
class MillerRabin {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool> && sizeof(T) <= 8);
    using U = std::make_unsigned_t<T>;

    static constexpr U multiply(U a, U b, U modulus) {
        if constexpr (sizeof(T) <= 4)
            return U(std::uint64_t(a) * b % modulus);
        else
            return U(static_cast<unsigned __int128>(a) * b % modulus);
    }

    static constexpr U power(U base, U exponent, U modulus) {
        U result = 1;
        for (; exponent; exponent >>= 1) {
            if (exponent & 1) result = multiply(result, base, modulus);
            base = multiply(base, base, modulus);
        }
        return result;
    }

public:
    // Deterministic throughout the input type's complete 32/64-bit range.
    constexpr bool operator()(T value) const {
        if (value < 2) return false;
        U n = U(value);
        for (U p : {U(2), U(3), U(5), U(7)}) {
            if (n % p == 0) return n == p;
        }
        U oddPart = n - 1;
        int twos = 0;
        while (!(oddPart & 1)) {
            oddPart >>= 1;
            ++twos;
        }
        auto test = [&](U base) constexpr {
            base %= n;
            if (base == 0) return true;
            U x = power(base, oddPart, n);
            if (x == 1 || x == n - 1) return true;
            for (int i = 1; i < twos; ++i) {
                x = multiply(x, x, n);
                if (x == n - 1) return true;
            }
            return false;
        };
        if constexpr (sizeof(T) <= 4) {
            for (U base : {U(2), U(7), U(61)})
                if (!test(base)) return false;
        } else {
            for (U base : {U(2), U(325), U(9375), U(28178), U(450775), U(9780504), U(1795265022)})
                if (!test(base)) return false;
        }
        return true;
    }
};

inline constexpr MillerRabin<long long> isPrime;
