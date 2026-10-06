#include "../../../../../src/Math/LinearAlgebra/LinearBasis/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<unsigned long long> a(n);
    for (auto &x : a)
        x = input.shape ? 1ULL << (random() % 64) : random();
    return measure(input, "full-width XOR inserts: random / dependent powers of two",
                   [&]() -> std::uint64_t {
                       LinearBasis<unsigned long long> b;
                       for (auto x : a)
                           b.insert(x);
                       return b.getMax() ^ b.rank;
                   });
}
