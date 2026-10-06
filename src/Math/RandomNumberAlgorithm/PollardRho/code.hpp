#pragma once
#include "Include.hpp"

// SNIPPET BEGIN
template <class T>
class PollardRho {
    static_assert(std::is_integral_v<T> and !std::is_same_v<T, bool> and sizeof(T) <= 8);
    using U = std::make_unsigned_t<T>;
    std::mt19937_64 rng;
    MillerRabin<T> mr{};

    static U mul(U a, U b, U mod) {
        if constexpr (sizeof(T) <= 4)
            return U(std::uint64_t(a) * b % mod);
        else
            return U(static_cast<unsigned __int128>(a) * b % mod);
    }

public:
    explicit PollardRho(
        std::uint64_t seed = std::chrono::steady_clock::now().time_since_epoch().count())
        : rng(seed) {}

    // Returns n iff n is prime; otherwise always a proper factor in [2, n).
    T findFactor(T a) {
        assert(a >= 2);
        U n = U(a);
        if (n % 2 == 0)
            return 2;
        if (n % 3 == 0)
            return 3;
        if (mr(a))
            return a;
        for (;;) {
            U c = U(rng() % (n - 1)) + 1, y = U(rng() % (n - 1)) + 1;
            auto step = [&](U x) {
                x = mul(x, x, n);
                return x >= n - c ? x - (n - c) : x + c;
            };
            U g = 1, x = 0, ys = 0;
            int bat = 0;
            for (int len = 1; g == 1 and len <= (1 << 20); len *= 2) {
                x = y;
                for (int i = 0; i < len; ++i)
                    y = step(y);
                for (int off = 0; off < len and g == 1; off += 128) {
                    ys = y;
                    U mulv = 1;
                    bat = std::min(128, len - off);
                    for (int i = 0; i < bat; ++i) {
                        y = step(y);
                        mulv = mul(mulv, x > y ? x - y : y - x, n);
                    }
                    g = std::gcd(mulv, n);
                }
            }
            if (g == n) {
                for (int i = 0; i < bat; ++i) {
                    ys = step(ys);
                    g = std::gcd(x > ys ? x - ys : ys - x, n);
                    if (g > 1)
                        break;
                }
            }
            if (g > 1 and g < n)
                return T(g);
        }
    }

    std::vector<std::pair<T, int>> primeFactorize(T x) {
        assert(x >= 1);
        std::vector<T> ps, stk;
        if (x > 1)
            stk.push_back(x);
        while (!stk.empty()) {
            x = stk.back();
            stk.pop_back();
            T p = findFactor(x);
            if (p == x)
                ps.push_back(x);
            else {
                stk.push_back(p);
                stk.push_back(x / p);
            }
        }
        std::sort(ps.begin(), ps.end());
        std::vector<std::pair<T, int>> fs;
        for (T p : ps) {
            if (fs.empty() or fs.back().first != p)
                fs.emplace_back(p, 1);
            else
                ++fs.back().second;
        }
        return fs;
    }
};

inline PollardRho<long long> rho;
