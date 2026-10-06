#include "../../../../../src/Math/MathPackage/ModuloInteger/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    using W = ModuloInteger<long long, 4179340454199820289LL>;
    std::vector<long long> a(n);
    for (auto &x : a)
        x = input.shape ? W::getMod() - 1 : static_cast<long long>(random() % W::getMod());
    return measure(input, "long-double modular multiplication: random / quotient-boundary",
                   [&]() -> std::uint64_t {
                       W x = 1;
                       for (auto v : a)
                           x = x * W(v) + W(v);
                       return x.val();
                   });
}
