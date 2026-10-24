#include "../../../../../src/extra/String/MinimalString/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::string expected(n, 'a');
    if (input.shape) {
        expected.back() = 'b';
    } else {
        expected[0] = '\0';
        for (int i = 1; i < n; ++i)
            expected[i] = char('a' + random() % 26);
    }
    // Both strings are already the unique minimal rotation before this shift.
    std::string text = expected;
    int shift = n == 1 ? 0 : 1 + int(random() % (n - 1));
    std::rotate(text.begin(), text.begin() + shift, text.end());
    return measure(input, "cyclic shift of random tail with unique minimum / near-uniform string",
                   [&]() -> std::uint64_t {
                       auto a = text | minimalString();
                       benchmarkCheck(a == expected, "known minimal rotation");
                       std::uint64_t sum = a.size();
                       for (unsigned char c : a)
                           sum = benchmarkMix(sum, c);
                       return sum;
                   });
}
