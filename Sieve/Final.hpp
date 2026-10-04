#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <vector>

// SNIPPET BEGIN
class Sieve {
    std::vector<int> minPrimeFactor{}, primeList{};

public:
    // The table covers [0, n); mpf(x) grows it automatically to cover x.
    explicit Sieve(std::size_t n = 0) { init(n); }

    static Sieve &shared(std::size_t n = 128) {
        static Sieve instance;
        instance.init(n);
        return instance;
    }

    void init(std::size_t n) {
        if (n <= size()) return;
        n = std::max(n, 2 * size());
        minPrimeFactor.assign(n, 0);
        primeList.clear();
        primeList.reserve(n / 10);
        for (std::size_t i = 2; i < n; ++i) {
            if (!minPrimeFactor[i]) {
                primeList.push_back(int(i));
                minPrimeFactor[i] = int(i);
            }
            for (int p : primeList) {
                auto prod = std::uint64_t(i) * p;
                if (prod >= n) break;
                minPrimeFactor[std::size_t(prod)] = p;
                if (p == minPrimeFactor[i]) break;
            }
        }
    }

    std::size_t size() const { return minPrimeFactor.size(); }

    int mpf(std::size_t x) {
        if (x >= size()) init(x + 1);
        return minPrimeFactor[x];
    }

    const std::vector<int> &primes() const { return primeList; }

    template <class T>
    std::vector<std::pair<T, int>> primeFactorize(T x) const {
        static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool> && sizeof(T) <= 8);
        assert(x >= 1);
        std::uint64_t rest = std::uint64_t(x);
        std::vector<std::pair<T, int>> ps;
        auto process = [&](std::uint64_t p) {
            int cnt = 0;
            do {
                rest /= p;
                ++cnt;
            } while (rest % p == 0);
            ps.emplace_back(T(p), cnt);
        };
        // Only trial-divide until the remainder fits the precomputed table.
        for (int p : primeList) {
            if (rest < size() || std::uint64_t(p) > rest / p) break;
            if (rest % p == 0) process(p);
        }
        // All smaller primes were tested; fallback supports an empty/small table.
        for (std::uint64_t d = std::max<std::uint64_t>(2, size()); rest >= size() && d <= rest / d;
             ++d) {
            if (rest % d == 0) process(d);
        }
        while (rest > 1 && rest < size()) {
            process(minPrimeFactor[std::size_t(rest)]);
        }
        if (rest > 1) ps.emplace_back(T(rest), 1);
        return ps;
    }

    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    std::vector<T> allFactors(T x) const {
        return allFactors(primeFactorize(x));
    }

    template <class T>
    static std::vector<T> allFactors(const std::vector<std::pair<T, int>> &ps) {
        std::vector<T> ds{T(1)};
        for (const auto &[prime, cnt] : ps) {
            auto old = ds.size();
            T pw = 1;
            for (int k = 1; k <= cnt; ++k) {
                pw *= prime;
                for (std::size_t i = 0; i < old; ++i)
                    ds.push_back(ds[i] * pw);
            }
        }
        return ds;
    }
};

inline auto &siv = Sieve::shared(10000);
