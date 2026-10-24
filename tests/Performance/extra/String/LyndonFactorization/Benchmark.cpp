#include "../../../../../src/extra/String/LyndonFactorization/code.hpp"
#include "../../../../../src/core/String/SuffixArray/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    std::mt19937_64 rng(input.seed);
    std::string s(input.n, '\0');
    for (int i = 0; i < input.n; ++i)
        s[i] = char(input.shape ? i % 7 : rng() % 256);
    std::vector<int> expected;
    if (input.shape) {
        // The increasing 7-byte word and its final proper prefix are Lyndon.
        for (int i = 0; i < input.n; i += 7)
            expected.push_back(i);
    } else {
        SuffixArray sa(s);
        int best = input.n;
        for (int i = 0; i < input.n; ++i) {
            // CFL factor starts are strict prefix minima of suffix ranks.
            if (sa.rk[i] < best) {
                best = sa.rk[i];
                expected.push_back(i);
            }
        }
    }
    expected.push_back(input.n);
    return measure(input, "Lyndon factorization: random bytes / repeated 7-byte word", [&] {
        auto pos = lyndonFactorization(s);
        benchmarkCheck(pos == expected, "suffix-array / known periodic factor boundaries");
        std::uint64_t sum = 0;
        for (int x : pos)
            sum = benchmarkMix(sum, x);
        return sum;
    });
}
