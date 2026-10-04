#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main() {
    const int n = 1000000;
    compare("linear-build", [&] {
        auto sieve = Legacy::Sieve::shared(); sieve.init(n);
        return std::uint64_t(sieve.primes().size()) + sieve.mpf(n - 1);
    }, [&] {
        Sieve sieve(n);
        return std::uint64_t(sieve.primes().size()) + sieve.mpf(n - 1);
    });
    auto old = Legacy::Sieve::shared(); old.init(n);
    Sieve current(n);
    std::mt19937 rng(17);
    std::vector<long long> values(100000);
    for (auto& x : values) x = rng() % (n - 1) + 1;
    compare("table-factorization", [&] {
        std::uint64_t sum = 0;
        for (auto x : values) for (auto [p, e] : old.primeFactorize(x)) sum += p * e;
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (auto x : values) for (auto [p, e] : current.primeFactorize(x)) sum += p * e;
        return sum;
    });
}
