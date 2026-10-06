#include "../../../../../src/Math/MathPackage/Sieve/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    (void)random;
    return measure(input, "sieve build and smallest-prime-factor queries", [&]() -> std::uint64_t {
        Sieve a(n);
        std::uint64_t sum = a.primes().size();
        for (int i = 1; i <= n; ++i)
            sum += a.mpf(i);
        return sum;
    });
}
