#include "../../../../../src/Math/Polynomial/NumberTheoreticTransform/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(const std::vector<Z> &a, const std::vector<Z> &b) {
    Poly x(a.begin(), a.end()), y(b.begin(), b.end());
    auto c = x * y;
    std::vector<Z> e(a.empty() or b.empty() ? 0 : a.size() + b.size() - 1);
    for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t j = 0; j < b.size(); ++j)
            e[i + j] += a[i] * b[j];
    CHECK(c.size() == e.size());
    for (std::size_t i = 0; i < e.size(); ++i)
        CHECK(c[i] == e[i]);
}

int main() {
    runCase("NumberTheoreticTransform/01-empty-both", [] {
        verifyAdded({}, {});
    });
    runCase("NumberTheoreticTransform/02-empty-right", [] {
        verifyAdded({1, 2, 3}, {});
    });
    runCase("NumberTheoreticTransform/03-scalar", [] {
        verifyAdded({-7}, {11});
    });
    runCase("NumberTheoreticTransform/04-trailing-zero", [] {
        verifyAdded({1, 0, 0}, {0, 0, 2});
    });
    runCase("NumberTheoreticTransform/05-negative-cancellation", [] {
        verifyAdded({1, -1, 1, -1}, {1, 1, 1, 1});
    });
    runCase("NumberTheoreticTransform/06-small-side-fallback", [] {
        verifyAdded(std::vector<Z>(127, 1), std::vector<Z>(257, -1));
    });
    runCase("NumberTheoreticTransform/07-exact-threshold", [] {
        verifyAdded(std::vector<Z>(128, 1), std::vector<Z>(128, 1));
    });
    runCase("NumberTheoreticTransform/08-power-boundary", [] {
        verifyAdded(std::vector<Z>(129, 1), std::vector<Z>(128, 2));
    });
    runCase("NumberTheoreticTransform/09-impulse", [] {
        std::vector<Z> a(257), b(129);
        a[256] = 3;
        b[64] = -2;
        verifyAdded(a, b);
    });
    runCase("NumberTheoreticTransform/10-cache-resize", [] {
        verifyAdded(std::vector<Z>(1025, 1), std::vector<Z>(129, -1));
        verifyAdded(std::vector<Z>(128, 3), std::vector<Z>(128, 2));
    });
    return finishCases(10);
}
