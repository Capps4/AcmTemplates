#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
class Sieve {
    std::vector<int> _mpf{}, _ps{};

public:
    // The table covers [0, n); mpf(x) grows it automatically to cover x.
    explicit Sieve(std::size_t n = 0) {
        init(n);
    }

    static Sieve &shared(std::size_t n = 128) {
        static Sieve obj;
        obj.init(n);
        return obj;
    }

    void init(std::size_t n) {
        if (n <= size())
            return;
        n = std::max(n, 2 * size());
        _mpf.assign(n, 0);
        _ps.clear();
        _ps.reserve(n / 10);
        for (std::size_t i = 2; i < n; ++i) {
            if (!_mpf[i]) {
                _ps.push_back(int(i));
                _mpf[i] = int(i);
            }
            for (int p : _ps) {
                auto prod = std::uint64_t(i) * p;
                if (prod >= n)
                    break;
                _mpf[std::size_t(prod)] = p;
                if (p == _mpf[i])
                    break;
            }
        }
    }

    std::size_t size() const {
        return _mpf.size();
    }

    int mpf(std::size_t x) {
        if (x >= size())
            init(x + 1);
        return _mpf[x];
    }

    const std::vector<int> &primes() const {
        return _ps;
    }

    template <class T>
    std::vector<std::pair<T, int>> primeFactorize(T x) const {
        static_assert(std::is_integral_v<T> and !std::is_same_v<T, bool> and sizeof(T) <= 8);
        assert(x >= 1);
        std::uint64_t rest = std::uint64_t(x);
        std::vector<std::pair<T, int>> ps;
        auto take = [&](std::uint64_t p) {
            int cnt = 0;
            do {
                rest /= p;
                ++cnt;
            } while (rest % p == 0);
            ps.emplace_back(T(p), cnt);
        };
        // Only trial-divide until the remainder fits the precomputed table.
        for (int p : _ps) {
            if (rest < size() or std::uint64_t(p) > rest / p)
                break;
            if (rest % p == 0)
                take(p);
        }
        // All smaller primes were tested; fallback supports an empty/small table.
        for (std::uint64_t d = std::max<std::uint64_t>(2, size()); rest >= size() and d <= rest / d;
             ++d) {
            if (rest % d == 0)
                take(d);
        }
        while (rest > 1 and rest < size()) {
            take(_mpf[std::size_t(rest)]);
        }
        if (rest > 1)
            ps.emplace_back(T(rest), 1);
        return ps;
    }

    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    std::vector<T> allFactors(T x) const {
        return allFactors(primeFactorize(x));
    }

    template <class T>
    static std::vector<T> allFactors(const std::vector<std::pair<T, int>> &ps) {
        std::vector<T> ds{T(1)};
        for (const auto &[v, cnt] : ps) {
            auto old = ds.size();
            T pw = 1;
            for (int k = 1; k <= cnt; ++k) {
                pw *= v;
                for (std::size_t i = 0; i < old; ++i)
                    ds.push_back(ds[i] * pw);
            }
        }
        return ds;
    }
};

inline auto &siv = Sieve::shared(int(1E5));
