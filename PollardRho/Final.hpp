#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <numeric>
#include <random>
#include <type_traits>
#include <vector>
#include "../MillerRabin/Final.hpp"

// SNIPPET BEGIN
template <class T>
class PollardRho {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool> && sizeof(T) <= 8);
    using U = std::make_unsigned_t<T>;
    std::mt19937_64 rng;
    MillerRabin<T> rabin{};

    static U multiply(U a, U b, U modulus) {
        if constexpr (sizeof(T) <= 4)
            return U(std::uint64_t(a) * b % modulus);
        else
            return U(static_cast<unsigned __int128>(a) * b % modulus);
    }

public:
    explicit PollardRho(
        std::uint64_t seed = std::chrono::steady_clock::now().time_since_epoch().count())
        : rng(seed) {}

    // Returns n iff n is prime; otherwise always a proper factor in [2, n).
    T findFactor(T value) {
        assert(value >= 2);
        U n = U(value);
        if (n % 2 == 0) return 2;
        if (n % 3 == 0) return 3;
        if (rabin(value)) return value;
        for (;;) {
            U c = U(rng() % (n - 1)) + 1, y = U(rng() % (n - 1)) + 1;
            auto step = [&](U x) {
                x = multiply(x, x, n);
                return x >= n - c ? x - (n - c) : x + c;
            };
            U divisor = 1, x = 0, savedY = 0;
            int batchLength = 0;
            for (int length = 1; divisor == 1 && length <= (1 << 20); length *= 2) {
                x = y;
                for (int i = 0; i < length; ++i)
                    y = step(y);
                for (int offset = 0; offset < length && divisor == 1; offset += 128) {
                    savedY = y;
                    U product = 1;
                    batchLength = std::min(128, length - offset);
                    for (int i = 0; i < batchLength; ++i) {
                        y = step(y);
                        product = multiply(product, x > y ? x - y : y - x, n);
                    }
                    divisor = std::gcd(product, n);
                }
            }
            if (divisor == n) {
                for (int i = 0; i < batchLength; ++i) {
                    savedY = step(savedY);
                    divisor = std::gcd(x > savedY ? x - savedY : savedY - x, n);
                    if (divisor > 1) break;
                }
            }
            if (divisor > 1 && divisor < n) return T(divisor);
        }
    }

    std::vector<std::pair<T, int>> primeFactorize(T x) {
        assert(x >= 1);
        std::vector<T> primes, stack;
        if (x > 1) stack.push_back(x);
        while (!stack.empty()) {
            x = stack.back();
            stack.pop_back();
            T factor = findFactor(x);
            if (factor == x)
                primes.push_back(x);
            else {
                stack.push_back(factor);
                stack.push_back(x / factor);
            }
        }
        std::sort(primes.begin(), primes.end());
        std::vector<std::pair<T, int>> factors;
        for (T p : primes) {
            if (factors.empty() || factors.back().first != p)
                factors.emplace_back(p, 1);
            else
                ++factors.back().second;
        }
        return factors;
    }
};

inline PollardRho<long long> rho;
