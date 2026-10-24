#include "../../../../../src/extra/Math/RandomNumber/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    auto seed = input.seed ^ (input.shape ? 1ULL << 63 : 0ULL);
    std::mt19937_64 reference(seed);
    std::uint64_t expected = 0;
    for (int i = 0; i < n; ++i)
        expected = benchmarkMix(expected, reference());
    return measure(input, "mt19937_64 seed replay: original / high-bit-flipped seed", [&]() -> std::uint64_t {
        rng.seed(seed);
        std::uint64_t sum = 0;
        for (int i = 0; i < n; ++i)
            sum = benchmarkMix(sum, rng());
        benchmarkCheck(sum == expected, "standard engine sequence");
        return sum;
    });
}
