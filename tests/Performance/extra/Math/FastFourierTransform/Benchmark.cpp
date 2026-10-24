#include "../../../../../src/extra/Math/FastFourierTransform/code.hpp"
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
    std::vector<int> positions{0, n / 2, n - 1, n + n / 2 - 1, 2 * n - 2};
    std::vector<long long> expected;
    for (int k : positions) {
        long long value = 0;
        for (int i = std::max(0, k - n + 1); i <= std::min(k, n - 1); ++i)
            value += static_cast<long long>(a[i]) * static_cast<long long>(b[k - i]);
        expected.push_back(value);
    }
    return measure(input, "equal-length convolution; random coefficients / impulse",
                   [&]() -> std::uint64_t {
                       Poly x(a.begin(), a.end()), y(b.begin(), b.end());
                       auto c = x * y;
                       benchmarkCheck(c.size() == std::size_t(2 * n - 1), "convolution length");
                       for (std::size_t i = 0; i < positions.size(); ++i)
                           benchmarkCheck(std::abs(c[positions[i]] - expected[i]) < 0.25,
                                          "direct coefficient oracle");
                       std::uint64_t sum = c.size();
                       for (int i = 0; i < int(c.size()); ++i) {
                           benchmarkCheck(std::isfinite(c[i]), "finite convolution coefficient");
                           if (input.shape)
                               benchmarkCheck(std::abs(c[i] - (i >= n - 1)) < 0.25,
                                              "exact impulse convolution");
                           sum = benchmarkMix(sum, std::uint64_t(std::llround(c[i])));
                       }
                       return sum;
                   });
}
