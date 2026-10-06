#include "../../../../../src/Math/Polynomial/FastFourierTransform/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<double> a(n), b(n);
    for (int i = 0; i < n; ++i) {
        a[i] = input.shape ? (i == n - 1) : int(random() % 101) - 50;
        b[i] = input.shape ? 1 : int(random() % 101) - 50;
    }
    return measure(input, "equal-length convolution; random coefficients / impulse",
                   [&]() -> std::uint64_t {
                       Poly x(a.begin(), a.end()), y(b.begin(), b.end());
                       auto c = x * y;
                       std::uint64_t sum = 0;
                       for (auto x : c)
                           sum ^= std::uint64_t(std::llround(x));
                       return sum;
                   });
}
