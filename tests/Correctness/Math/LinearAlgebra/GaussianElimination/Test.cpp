#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/LinearAlgebra/GaussianElimination/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../../src/Math/MathPackage/ModuloInteger/code.hpp"
#include <complex>

using Small = ModuloInteger<int, 3>;
int coreCases() {
    std::vector<std::vector<double>> empty;
    CHECK(gauss(empty) == "OK");
    MatrixUtil<double> emptyInverse(empty);
    CHECK(emptyInverse.status == "OK" && emptyInverse.inv.empty());
    std::vector<std::vector<double>> manyRight{{1, 0, 0, 0}, {0, 0, 0, 1}};
    CHECK(gauss(manyRight) == "NoSolution"); // Only the second RHS is inconsistent.
    MatrixUtil<double> singular({{1, 0}, {0, 0}});
    CHECK(singular.status == "NoSolution" && singular.inv.empty());
    for (int code = 0; code < 3 * 3 * 3 * 3; ++code) {
        test_context::step = code;
        int value = code;
        std::vector<std::vector<Small>> coefficients(2, std::vector<Small>(2));
        for (auto& row : coefficients) for (auto& entry : row) { entry = value % 3; value /= 7; }
        MatrixUtil<Small> inverse(coefficients);
        auto determinant = coefficients[0][0] * coefficients[1][1] - coefficients[0][1] * coefficients[1][0];
        CHECK((inverse.status == "OK") == (determinant != Small{}));
        if (inverse.status == "OK") for (int x = 0; x < 2; ++x) for (int y = 0; y < 2; ++y) {
            Small product = 0;
            for (int k = 0; k < 2; ++k) product += coefficients[x][k] * inverse.inv[k][y];
            CHECK(product == Small(x == y));
        }
        for (int rhs0 = 0; rhs0 < 3; ++rhs0) for (int rhs1 = 0; rhs1 < 3; ++rhs1) {
            int solutions = 0, answer0 = -1, answer1 = -1;
            for (int x = 0; x < 3; ++x) for (int y = 0; y < 3; ++y)
                if (coefficients[0][0] * x + coefficients[0][1] * y == Small(rhs0) &&
                    coefficients[1][0] * x + coefficients[1][1] * y == Small(rhs1)) {
                    ++solutions; answer0 = x; answer1 = y;
                }
            auto augmented = coefficients;
            augmented[0].push_back(rhs0); augmented[1].push_back(rhs1);
            auto status = gauss(augmented);
            CHECK(status == (solutions == 0 ? "NoSolution" : solutions == 1 ? "OK" : "InfSolution"));
            if (solutions == 1) CHECK(augmented[0][2] == Small(answer0) && augmented[1][2] == Small(answer1));
        }
    }
    for (int trial = 0; trial < 8; ++trial) {
        test_context::step = trial;
        int n = randomInt(1, 15);
        std::vector<std::vector<double>> matrix(n, std::vector<double>(n)), augmented;
        std::vector<double> answers(n);
        for (auto& value : answers) value = randomInt(-20, 20);
        for (int x = 0; x < n; ++x) for (int y = 0; y < n; ++y)
            matrix[x][y] = x == y ? n * 10.0 : randomInt(-4, 4);
        augmented = matrix;
        for (int x = 0; x < n; ++x) {
            double right = 0;
            for (int y = 0; y < n; ++y) right += matrix[x][y] * answers[y];
            augmented[x].push_back(right);
        }
        CHECK(gauss(augmented) == "OK");
        for (int x = 0; x < n; ++x) CHECK(std::abs(augmented[x][n] - answers[x]) < 1e-9);
        MatrixUtil<double> inverse(matrix);
        CHECK(inverse.status == "OK");
        for (int x = 0; x < n; ++x) for (int y = 0; y < n; ++y) {
            double left = 0, right = 0;
            for (int k = 0; k < n; ++k) {
                left += matrix[x][k] * inverse.inv[k][y];
                right += inverse.inv[x][k] * matrix[k][y];
            }
            CHECK(std::abs(left - (x == y)) < 1e-9 && std::abs(right - (x == y)) < 1e-9);
        }
    }
    std::vector<std::vector<double>> unstable{{1, 1, 2}, {1e-30, 1, 1}};
    CHECK(gauss(unstable) == "OK" && std::abs(unstable[0][2] - 1) < 1e-12 && std::abs(unstable[1][2] - 1) < 1e-12);
    for (double scale : {1e-310, 1e308}) {
        std::vector<std::vector<double>> tiny{{scale, scale}};
        CHECK(gauss(tiny) == "OK" && tiny[0][1] == 1);
    }
    using Complex = std::complex<double>;
    std::vector<std::vector<Complex>> complex{{Complex(1, 2), Complex(3, 6)}};
    CHECK(gauss(complex) == "OK" && std::abs(complex[0][1] - Complex(3)) < 1e-12);
    std::cout << "GaussianElimination finite-field exhaustive solution oracle, two-sided inverse, pivot stability, empty/multi-RHS PASS\n";
    return 0;
}

#include "../../../../../src/Math/LinearAlgebra/GaussianElimination/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
void verifyAdded(const std::vector<std::vector<double>> &a, bool invertible) {
    MatrixUtil<double> inv(a);
    CHECK((inv.status == "OK") == invertible);
    if (!invertible) {
        CHECK(inv.inv.empty());
        return;
    }
    int n = int(a.size());
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            long double v = 0;
            for (int k = 0; k < n; ++k)
                v += (long double)a[i][k] * inv.inv[k][j];
            CHECK(std::abs(v - (i == j)) < 1e-8L);
        }
}

int run() {
    runCase("GaussianElimination/empty", [] {
        verifyAdded({}, true);
    });
    runCase("GaussianElimination/identity", [] {
        verifyAdded({{1, 0}, {0, 1}}, true);
    });
    runCase("GaussianElimination/swap-pivot", [] {
        verifyAdded({{0, 2}, {3, 4}}, true);
    });
    runCase("GaussianElimination/zero", [] {
        verifyAdded({{0, 0}, {0, 0}}, false);
    });
    runCase("GaussianElimination/dependent", [] {
        verifyAdded({{1, 2}, {2, 4}}, false);
    });
    runCase("GaussianElimination/diagonal", [] {
        verifyAdded({{2, 0, 0}, {0, 3, 0}, {0, 0, 4}}, true);
    });
    runCase("GaussianElimination/signed", [] {
        verifyAdded({{1, -2}, {3, 4}}, true);
    });
    runCase("GaussianElimination/triangular", [] {
        verifyAdded({{1, 2, 3}, {0, 1, 4}, {0, 0, 1}}, true);
    });
    runCase("GaussianElimination/fractional", [] {
        verifyAdded({{0.5, 1}, {1, 3}}, true);
    });
    runCase("GaussianElimination/near-scale", [] {
        verifyAdded({{1e-20, 0}, {0, 1e-20}}, true);
    });
    return 0;
}
}

int main() {
    runCase("GaussianElimination/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
