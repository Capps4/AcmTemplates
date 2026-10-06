#include "../../../../../src/String/StringF4/MinimalString/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::string text(n, 'a');
    for (int i = 0; i < n; ++i)
        text[i] = input.shape ? 'a' : char('a' + random() % 26);

    return measure(input, "random-alphabet / repetitive input; build and public queries",
                   [&]() -> std::uint64_t {
                       auto a = text | minimalString();
                       return std::accumulate(a.begin(), a.end(), std::uint64_t(0));
                   });
}
