#include "../../../../../src/Math/RandomNumberAlgorithm/MillerRabin/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<unsigned long long> a(n);
    for (auto &x : a)
        x = input.shape ? 2305843009213693951ULL : ((random() >> 1) | 1);
    return measure(input, "random odd 63-bit integers / large prime", [&]() -> std::uint64_t {
        std::uint64_t sum = 0;
        MillerRabin<unsigned long long> test;
        for (auto x : a)
            sum += test(x);
        return sum;
    });
}
