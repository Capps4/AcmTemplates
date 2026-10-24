#include "../../../../../src/extra/Math/GaussianElimination/code.hpp"
#include "../../../../../src/core/Math/Numeric/ModuloInteger/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    int m = input.n, n = input.shape ? std::max(1, m / 2) : m * 2, q = 4;
    int rank = std::min(n, m);
    std::mt19937_64 rng(input.seed);
    std::vector<std::vector<Z>> a(n, std::vector<Z>(m + q));
    for (int i = 0; i < rank; ++i) {
        a[i][i] = 1;
        for (int j = i + 1; j < m; ++j)
            a[i][j] = rng();
    }
    for (int i = rank; i < n; ++i)
        for (int j = 0; j < m; ++j)
            a[i][j] = a[i % rank][j];
    std::vector<std::vector<Z>> x(q, std::vector<Z>(m));
    for (auto &row : x)
        for (int j = 0; j < rank; ++j)
            row[j] = rng();
    for (int i = 0; i < n; ++i)
        for (int k = 0; k < q; ++k)
            for (int j = 0; j < m; ++j)
                a[i][m + k] += a[i][j] * x[k][j];
    // Invertible row additions preserve rank, pivots and the known solutions.
    // Two opposite sweeps spread every original row through the dense matrix.
    for (int i = 1; i < n; ++i) {
        Z factor = rng();
        if (factor == Z{})
            factor = 1;
        for (int j = 0; j < m + q; ++j)
            a[i][j] += factor * a[i - 1][j];
    }
    for (int i = n - 2; i >= 0; --i) {
        Z factor = rng();
        if (factor == Z{})
            factor = 1;
        for (int j = 0; j < m + q; ++j)
            a[i][j] += factor * a[i + 1][j];
    }
    std::shuffle(a.begin(), a.end(), rng);
    return measure(input, "finite field rectangular solve: tall / wide, four RHS", [&] {
        auto b = a;
        auto result = gauss(b, m);
        benchmarkCheck(result.rank == rank and result.sol == x, "rank and canonical particular solutions");
        std::uint64_t sum = 0;
        for (const auto &row : result.sol)
            for (auto value : row)
                sum = benchmarkMix(sum, value.val());
        return sum;
    });
}
