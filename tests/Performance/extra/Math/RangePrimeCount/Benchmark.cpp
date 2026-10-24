#include "../../../../../src/extra/Math/RangePrimeCount/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    long long n = 1LL * in.n * in.n + (in.shape ? 1 : -1);
    // A plain sieve is independent of the quotient-table counting algorithm.
    std::vector<bool> prime(std::size_t(n) + 1, true);
    prime[0] = false;
    if (n >= 1)
        prime[1] = false;
    for (long long p = 2; p <= n / p; ++p)
        if (prime[std::size_t(p)])
            for (long long x = p * p; x <= n; x += p)
                prime[std::size_t(x)] = false;
    auto expected = std::count(prime.begin(), prime.end(), true);
    return measure(in, "prime counting immediately below / above a square", [&]() -> std::uint64_t {
        auto v = cntPrimeLessEqual(n);
        benchmarkCheck(v == expected, "independent sieve count");
        return std::uint64_t(v);
    });
}
