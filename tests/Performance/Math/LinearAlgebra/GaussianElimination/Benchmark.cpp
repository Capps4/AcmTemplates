#include "../../../../../src/Math/LinearAlgebra/GaussianElimination/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::vector<double>> a(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            a[i][j] = i == j ? n * 2. : input.shape ? 0. : double(int(random() % 7) - 3);
    return measure(input, "dense diagonally dominant / diagonal matrix inversion",
                   [&]() -> std::uint64_t {
                       MatrixUtil<double> b(a);
                       if (b.status != "OK")
                           std::exit(1);
                       std::uint64_t sum = 0;
                       for (int i = 0; i < n; ++i)
                           sum += std::uint64_t(std::abs(b.inv[i][i]) * 1e12);
                       return sum;
                   });
}
