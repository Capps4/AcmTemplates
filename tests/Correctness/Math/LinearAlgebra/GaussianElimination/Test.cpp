#include "../../../../../src/Math/LinearAlgebra/GaussianElimination/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../../src/Math/MathPackage/ModuloInteger/code.hpp"
#include <complex>
using Small = ModuloInteger<int, 7>;
int main() {
    std::vector<std::vector<double>> empty;
    CHECK(gauss(empty) == "OK");
    MatrixUtil<double> emptyInverse(empty);
    CHECK(emptyInverse.status == "OK" && emptyInverse.inv.empty());
    std::vector<std::vector<double>> manyRight{{1, 0, 0, 0}, {0, 0, 0, 1}};
    CHECK(gauss(manyRight) == "NoSolution"); // Only the second RHS is inconsistent.
    MatrixUtil<double> singular({{1, 0}, {0, 0}});
    CHECK(singular.status == "NoSolution" && singular.inv.empty());
    for (int code = 0; code < 7 * 7 * 7 * 7; ++code) {
        int value = code;
        std::vector<std::vector<Small>> coefficients(2, std::vector<Small>(2));
        for (auto& row : coefficients) for (auto& entry : row) { entry = value % 7; value /= 7; }
        MatrixUtil<Small> inverse(coefficients);
        auto determinant = coefficients[0][0] * coefficients[1][1] - coefficients[0][1] * coefficients[1][0];
        CHECK((inverse.status == "OK") == (determinant != Small{}));
        if (inverse.status == "OK") for (int x = 0; x < 2; ++x) for (int y = 0; y < 2; ++y) {
            Small product = 0;
            for (int k = 0; k < 2; ++k) product += coefficients[x][k] * inverse.inv[k][y];
            CHECK(product == Small(x == y));
        }
        for (int rhs0 = 0; rhs0 < 7; ++rhs0) for (int rhs1 = 0; rhs1 < 7; ++rhs1) {
            int solutions = 0, answer0 = -1, answer1 = -1;
            for (int x = 0; x < 7; ++x) for (int y = 0; y < 7; ++y)
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
    for (int trial = 0; trial < 300; ++trial) {
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
}
