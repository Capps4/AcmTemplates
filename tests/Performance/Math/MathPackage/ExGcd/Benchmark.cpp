#include "../../../../../src/Math/MathPackage/ExGcd/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::pair<long long, long long>> a(n);
    for (auto &p : a)
        p = {1 + static_cast<long long>(random() % 1000000000000ULL),
             input.shape ? 514229LL : 1 + static_cast<long long>(random() % 1000000000000ULL)};
    return measure(input, "Bezout on random pairs / Fibonacci denominator", [&]() -> std::uint64_t {
        std::uint64_t sum = 0;
        for (auto [a, b] : a) {
            long long x = 0, y = 0;
            sum += exgcd(a, b, x, y);
            sum ^= std::uint64_t(x) ^ std::uint64_t(y);
        }
        return sum;
    });
}
