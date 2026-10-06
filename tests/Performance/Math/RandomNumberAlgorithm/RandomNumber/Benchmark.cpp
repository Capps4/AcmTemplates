#include "../../../../../src/Math/RandomNumberAlgorithm/RandomNumber/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    (void)random;
    return measure(input, "mt19937_64 generation with seed replay", [&]() -> std::uint64_t {
        rng.seed(input.seed);
        std::uint64_t sum = 0;
        for (int i = 0; i < n; ++i)
            sum ^= rng();
        return sum;
    });
}
