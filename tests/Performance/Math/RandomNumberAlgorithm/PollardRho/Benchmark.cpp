#include "../../../../../src/Math/RandomNumberAlgorithm/PollardRho/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<unsigned long long> a(n);
    for (auto &x : a)
        x = input.shape ? 1000036000099ULL : 2 + random() % 1000000000000ULL;
    return measure(input, "random integers / close-prime semiprime factoring",
                   [&]() -> std::uint64_t {
                       PollardRho<unsigned long long> f(input.seed);
                       std::uint64_t sum = 0;
                       for (auto x : a) {
                           unsigned long long remaining = x;
                           for (auto [p, e] : f.primeFactorize(x)) {
                               benchmarkCheck(p > 1 and e > 0, "positive prime powers");
                               for (int i = 0; i < e; ++i) {
                                   benchmarkCheck(remaining % p == 0, "factor divides remaining input");
                                   remaining /= p;
                               }
                               sum = benchmarkMix(sum, p + e);
                           }
                           benchmarkCheck(remaining == 1, "factorization reconstructs input");
                       }
                       return sum;
                   });
}
