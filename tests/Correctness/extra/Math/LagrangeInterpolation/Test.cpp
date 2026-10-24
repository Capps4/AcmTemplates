#include "../../../../../src/extra/Math/LagrangeInterpolation/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

template <class T>
T eval(const std::vector<T> &a, T x) {
    T res = 0;
    for (auto it = a.rbegin(); it != a.rend(); ++it)
        res = res * x + *it;
    return res;
}

template <class T>
void verify(const std::vector<T> &a, const std::vector<T> &xs) {
    std::vector<T> y(a.size());
    for (int i = 0; i < int(a.size()); ++i)
        y[i] = eval(a, T(i));
    const auto saved = y;
    for (T x : xs)
        CHECK(lagrangeInterpolation(y, x) == eval(a, x));
    CHECK(y == saved);
}

int main() {
    runCase("LagrangeInterpolation/small-field-exhaustive", [] {
        using T = ModuloInteger<int, 5>;
        std::vector<T> xs{0, 1, 2, 3, 4};
        int count = 1;
        for (int n = 0; n <= 5; ++n) {
            for (int mask = 0; mask < count; ++mask) {
                test_context::step = mask;
                int bits = mask;
                std::vector<T> a(n);
                for (auto &v : a) {
                    v = bits % 5;
                    bits /= 5;
                }
                verify(a, xs);
            }
            count *= 5;
        }
    });
    runCase("LagrangeInterpolation/random-polynomials", [] {
        for (int trial = 0; trial < 120; ++trial) {
            test_context::step = trial;
            int n = randomInt(0, 120);
            std::vector<Z> a(n), xs;
            for (auto &v : a)
                v = randomInt(-1000000000, 1000000000);
            // The small-field case already checks every sampling point.
            xs.push_back(Z(0));
            xs.push_back(Z(n / 2));
            xs.push_back(Z(std::max(0, n - 1)));
            xs.push_back(Z(-1));
            xs.push_back(Z(n));
            xs.push_back(Z(LLONG_MIN));
            xs.push_back(Z(LLONG_MAX));
            for (int i = 0; i < 10; ++i)
                xs.push_back(Z(testRng()));
            verify(a, xs);
        }
    });
    runCase("LagrangeInterpolation/modulus-switch-and-large", [] {
        using T = ModuloInteger<int, 0>;
        for (int p : {101, 7, 1009, 5, 101}) {
            T::setMod(p);
            std::vector<T> a(std::min(p - 1, 40));
            for (auto &v : a)
                v = randomInt(0, p - 1);
            verify(a, std::vector<T>{T(0), T(p / 2), T(p - 1)});
        }
        using U = ModuloInteger<long long, 2305843009213693951LL>;
        verify(std::vector<U>{U(LLONG_MIN), U(7), U(-3), U(LLONG_MAX)},
               std::vector<U>{U(0), U(3), U(-1), U(LLONG_MIN), U(LLONG_MAX)});
        std::vector<Z> y(100000);
        for (int i = 0; i < int(y.size()); ++i)
            y[i] = Z(i) * i + 7;
        Z x = 987654321;
        CHECK(lagrangeInterpolation(y, x) == x * x + 7);
    });
    return 0;
}
