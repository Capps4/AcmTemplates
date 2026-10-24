#include "../../../../../src/extra/Math/GaussianElimination/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::vector<double>> a(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i) {
        double offDiagonal = 0;
        for (int j = 0; j < n; ++j) {
            if (i == j)
                continue;
            a[i][j] = input.shape ? 0. : double(int(random() % 7) - 3);
            offDiagonal += std::abs(a[i][j]);
        }
        a[i][i] = offDiagonal + n * 2.;
    }
    auto checksum = [](const std::vector<std::vector<double>> &inverse) {
        std::uint64_t sum = 0;
        for (const auto &row : inverse)
            for (double value : row) {
                benchmarkCheck(std::isfinite(value), "finite inverse coefficient");
                sum = benchmarkMix(sum, std::uint64_t(std::llround(value * 1e12)));
            }
        return sum;
    };
    MatrixUtil<double> reference(a);
    benchmarkCheck(reference.status == "OK", "strictly diagonally dominant matrix is invertible");
    // Check every entry of A*inverse against I outside the timed inversion.
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            long double value = 0;
            for (int k = 0; k < n; ++k)
                value += static_cast<long double>(a[i][k]) * reference.inv[k][j];
            benchmarkCheck(std::abs(value - (i == j)) < 1e-10L, "inverse identity residual");
        }
    auto expected = checksum(reference.inv);
    return measure(input, "dense diagonally dominant / diagonal matrix inversion",
                   [&]() -> std::uint64_t {
                       MatrixUtil<double> b(a);
                       benchmarkCheck(b.status == "OK", "matrix inversion status");
                       auto sum = checksum(b.inv);
                       benchmarkCheck(sum == expected, "residual-validated ordered inverse checksum");
                       return sum;
                   });
}
