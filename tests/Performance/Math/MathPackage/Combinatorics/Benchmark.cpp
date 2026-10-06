#include "../../../../../src/Math/MathPackage/Combinatorics/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> q(n);
    for (auto &x : q)
        x = input.shape ? n / 2 : random() % (n + 1);
    return measure(input, "factorial cache build and binomial queries", [&]() -> std::uint64_t {
        Comb<Z> c(n);
        std::uint64_t sum = 0;
        for (int x : q)
            sum += c.C(n, x).val();
        return sum;
    });
}
