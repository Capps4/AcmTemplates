#include "../../../../../src/extra/Math/ModSqrt/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

using i64 = long long;

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    int p = input.shape ? 2013265921 : 1000000007;
    std::mt19937_64 rng(input.seed);
    std::vector<int> values(input.n), expected(input.n);
    for (int i = 0; i < input.n; ++i) {
        i64 x = rng() % p;
        values[i] = int(x * x % p);
        expected[i] = int(std::min(x, p - x));
    }
    std::uint64_t target = 0;
    for (int value : expected)
        target = benchmarkMix(target, value);
    return measure(input, "modular roots: p=1e9+7 / p=15*2^27+1", [&] {
        std::uint64_t sum = 0;
        for (int a : values)
            sum = benchmarkMix(sum, modSqrt(a, p));
        benchmarkCheck(sum == target, "independently constructed canonical roots");
        return sum;
    });
}
