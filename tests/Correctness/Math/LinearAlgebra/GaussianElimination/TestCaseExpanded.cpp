#include "../../../../../src/Math/LinearAlgebra/GaussianElimination/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
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

int main() {
    runCase("GaussianElimination/01-empty", [] {
        verifyAdded({}, true);
    });
    runCase("GaussianElimination/02-identity", [] {
        verifyAdded({{1, 0}, {0, 1}}, true);
    });
    runCase("GaussianElimination/03-swap-pivot", [] {
        verifyAdded({{0, 2}, {3, 4}}, true);
    });
    runCase("GaussianElimination/04-zero", [] {
        verifyAdded({{0, 0}, {0, 0}}, false);
    });
    runCase("GaussianElimination/05-dependent", [] {
        verifyAdded({{1, 2}, {2, 4}}, false);
    });
    runCase("GaussianElimination/06-diagonal", [] {
        verifyAdded({{2, 0, 0}, {0, 3, 0}, {0, 0, 4}}, true);
    });
    runCase("GaussianElimination/07-signed", [] {
        verifyAdded({{1, -2}, {3, 4}}, true);
    });
    runCase("GaussianElimination/08-triangular", [] {
        verifyAdded({{1, 2, 3}, {0, 1, 4}, {0, 0, 1}}, true);
    });
    runCase("GaussianElimination/09-fractional", [] {
        verifyAdded({{0.5, 1}, {1, 3}}, true);
    });
    runCase("GaussianElimination/10-near-scale", [] {
        verifyAdded({{1e-20, 0}, {0, 1e-20}}, true);
    });
    return finishCases(10);
}
