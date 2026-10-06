#include "../../../../../src/Math/MathPackage/FloatPointNumber/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<double> a(n);
    for (auto &x : a)
        x = input.shape ? 0.4999999999999 : double(random() % 100000) / 37;
    return measure(input, "wrapper arithmetic and std::sqrt: random / near rounding boundary",
                   [&]() -> std::uint64_t {
                       Float sum = 0;
                       for (double x : a)
                           sum += std::sqrt(Float(x + 1));
                       return static_cast<std::uint64_t>(sum.val());
                   });
}
