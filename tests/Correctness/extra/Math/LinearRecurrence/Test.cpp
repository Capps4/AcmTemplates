#include "../../../../../src/extra/Math/LinearRecurrence/code.hpp"
#include "../../../../../src/core/Math/Numeric/ModuloInteger/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using Small = ModuloInteger<int, 3>;

template <class T>
std::vector<T> extend(std::vector<T> a, const std::vector<T> &c, int n) {
    while (int(a.size()) < n) {
        T value{};
        for (std::size_t j = 0; j < c.size(); ++j)
            value += c[j] * a[a.size() - j - 1];
        a.push_back(value);
    }
    return a;
}

template <class T>
bool fits(const std::vector<T> &a, const std::vector<T> &c) {
    for (std::size_t i = c.size(); i < a.size(); ++i) {
        T value{};
        for (std::size_t j = 0; j < c.size(); ++j)
            value += c[j] * a[i - j - 1];
        if (value != a[i])
            return false;
    }
    return true;
}

std::pair<Z, Z> fibonacci(std::uint64_t k) {
    if (k == 0)
        return {0, 1};
    auto [a, b] = fibonacci(k / 2);
    Z c = a * (b + b - a), d = a * a + b * b;
    return k & 1 ? std::make_pair(d, c + d) : std::make_pair(c, d);
}

int main() {
    runCase("LinearRecurrence/small-field-exhaustive", [] {
        for (int d = 1; d <= 3; ++d) {
            int count = 1;
            for (int i = 0; i < 2 * d; ++i)
                count *= 3;
            for (int code = 0; code < count; ++code) {
                int value = code;
                std::vector<Small> a(d), c(d);
                for (auto &x : a) {
                    x = value % 3;
                    value /= 3;
                }
                for (auto &x : c) {
                    x = value % 3;
                    value /= 3;
                }
                auto expected = extend(a, c, 24);
                auto inferred = berlekampMassey(std::vector<Small>(expected.begin(), expected.begin() + 2 * d));
                for (int k = 0; k < int(expected.size()); ++k) {
                    CHECK(linearRecurrence(a, c, k) == expected[k]);
                    CHECK(linearRecurrence(a, inferred, k) == expected[k]);
                }
            }
        }
    });
    runCase("LinearRecurrence/minimal-order-oracle", [] {
        for (int n = 0; n <= 6; ++n) {
            int count = 1;
            for (int i = 0; i < n; ++i)
                count *= 3;
            for (int code = 0; code < count; ++code) {
                int value = code;
                std::vector<Small> a(n);
                for (auto &x : a) {
                    x = value % 3;
                    value /= 3;
                }
                auto c = berlekampMassey(a);
                CHECK(fits(a, c));
                int choices = 1;
                for (int d = 0; d < int(c.size()); ++d) {
                    for (int mask = 0; mask < choices; ++mask) {
                        int digits = mask;
                        std::vector<Small> b(d);
                        for (auto &x : b) {
                            x = digits % 3;
                            digits /= 3;
                        }
                        CHECK(!fits(a, b));
                    }
                    choices *= 3;
                }
            }
        }
    });
    runCase("LinearRecurrence/random-recovery", [] {
        for (int trial = 0; trial < 80; ++trial) {
            test_context::step = trial;
            int d = randomInt(1, 16);
            std::vector<Z> a(d), c(d);
            for (auto &x : a)
                x = testRng();
            for (auto &x : c)
                x = testRng();
            auto expected = extend(a, c, 100);
            auto inferred = berlekampMassey(std::vector<Z>(expected.begin(), expected.begin() + 2 * d));
            CHECK(inferred.size() <= c.size());
            for (int k : {0, d - 1, d, 2 * d, 73, 99}) {
                CHECK(linearRecurrence(a, c, k) == expected[k]);
                CHECK(linearRecurrence(a, inferred, k) == expected[k]);
            }
        }
    });
    runCase("LinearRecurrence/empty-and-uint64-index", [] {
        auto hi = std::numeric_limits<std::uint64_t>::max();
        CHECK(linearRecurrence<Z>({}, {}, hi) == Z{});
        CHECK(berlekampMassey(std::vector<Z>(20)).empty());
        CHECK(linearRecurrence<Z>({7}, {}, 0) == Z(7));
        CHECK(linearRecurrence<Z>({7}, {}, 1) == Z{});
        std::vector<Z> c(7), a{0, 1, 2, 3, 4, 5, 6};
        c.back() = 1;
        for (std::uint64_t k : {std::uint64_t(0), std::uint64_t(1) << 63, hi}) {
            CHECK(linearRecurrence<Z>({0, 1}, {1, 1}, k) == fibonacci(k).first);
            CHECK(linearRecurrence<Z>({3}, {5}, k) == Z(3) * Z(5).power(k % (P - 1)));
            CHECK(linearRecurrence(a, c, k) == a[k % 7]);
        }
    });
    return 0;
}
