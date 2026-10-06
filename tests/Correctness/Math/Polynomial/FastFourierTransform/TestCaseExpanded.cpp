#include "../../../../../src/Math/Polynomial/FastFourierTransform/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(const std::vector<double> &a, const std::vector<double> &b) {
    Poly x(a.begin(), a.end()), y(b.begin(), b.end());
    auto c = x * y;
    std::vector<double> e(a.empty() or b.empty() ? 0 : a.size() + b.size() - 1);
    for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t j = 0; j < b.size(); ++j)
            e[i + j] += a[i] * b[j];
    CHECK(c.size() == e.size());
    for (std::size_t i = 0; i < e.size(); ++i)
        CHECK(std::abs(c[i] - e[i]) < 1e-7 * (1 + std::abs(e[i])));
}

int main() {
    runCase("FastFourierTransform/01-empty-both", [] {
        verifyAdded({}, {});
    });
    runCase("FastFourierTransform/02-empty-right", [] {
        verifyAdded({1, 2, 3}, {});
    });
    runCase("FastFourierTransform/03-scalar", [] {
        verifyAdded({-7}, {11});
    });
    runCase("FastFourierTransform/04-trailing-zero", [] {
        verifyAdded({1, 0, 0}, {0, 0, 2});
    });
    runCase("FastFourierTransform/05-negative-cancellation", [] {
        verifyAdded({1, -1, 1, -1}, {1, 1, 1, 1});
    });
    runCase("FastFourierTransform/06-small-side-fallback", [] {
        verifyAdded(std::vector<double>(127, 1), std::vector<double>(257, -1));
    });
    runCase("FastFourierTransform/07-exact-threshold", [] {
        verifyAdded(std::vector<double>(128, 1), std::vector<double>(128, 1));
    });
    runCase("FastFourierTransform/08-power-boundary", [] {
        verifyAdded(std::vector<double>(129, 1), std::vector<double>(128, 2));
    });
    runCase("FastFourierTransform/09-impulse", [] {
        std::vector<double> a(257), b(129);
        a[256] = 3;
        b[64] = -2;
        verifyAdded(a, b);
    });
    runCase("FastFourierTransform/10-cache-resize", [] {
        verifyAdded(std::vector<double>(1025, 1), std::vector<double>(129, -1));
        verifyAdded(std::vector<double>(128, 3), std::vector<double>(128, 2));
    });
    return finishCases(10);
}
