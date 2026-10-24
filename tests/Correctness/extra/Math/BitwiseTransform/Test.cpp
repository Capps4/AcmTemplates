#include "../../../../../src/extra/Math/BitwiseTransform/code.hpp"
#include "../../../../../src/core/Math/Numeric/ModuloInteger/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

template <BitwiseOp Op, class T>
void verify(const std::vector<T> &a, const std::vector<T> &b) {
    std::vector<T> expected(a.size());
    for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t j = 0; j < b.size(); ++j) {
            std::size_t k = Op == BitwiseOp::Xor ? i ^ j : Op == BitwiseOp::And ? i & j : i | j;
            expected[k] += a[i] * b[j];
        }
    CHECK(bitwiseConvolution<Op>(a, b) == expected);
    auto roundtrip = a;
    bitwiseTransform<Op>(roundtrip);
    bitwiseTransform<Op>(roundtrip, true);
    CHECK(roundtrip == a);
}

template <class T>
void all(const std::vector<T> &a, const std::vector<T> &b) {
    verify<BitwiseOp::Xor>(a, b);
    verify<BitwiseOp::And>(a, b);
    verify<BitwiseOp::Or>(a, b);
}

int main() {
    runCase("BitwiseTransform/integer-exhaustive", [] {
        all<long long>({}, {});
        all<long long>({7}, {-3});
        const auto lo = std::numeric_limits<long long>::lowest();
        std::vector<long long> a{0, lo}, b{lo, 0};
        bitwiseTransform<BitwiseOp::And>(a);
        bitwiseTransform<BitwiseOp::And>(a, true);
        bitwiseTransform<BitwiseOp::Or>(b);
        bitwiseTransform<BitwiseOp::Or>(b, true);
        CHECK(a == std::vector<long long>({0, lo}));
        CHECK(b == std::vector<long long>({lo, 0}));
        for (int code = 0; code < 81; ++code) {
            int value = code;
            std::vector<long long> a(2), b(2);
            for (auto &x : a) {
                x = value % 3 - 1;
                value /= 3;
            }
            for (auto &x : b) {
                x = value % 3 - 1;
                value /= 3;
            }
            all(a, b);
        }
    });
    runCase("BitwiseTransform/field-random-oracle", [] {
        for (int trial = 0; trial < 160; ++trial) {
            test_context::step = trial;
            int n = 1 << randomInt(0, 6);
            std::vector<Z> a(n), b(n);
            for (auto &x : a)
                x = randomInt(-100000, 100000);
            for (auto &x : b)
                x = randomInt(-100000, 100000);
            all(a, b);
        }
    });
    runCase("BitwiseTransform/large-impulse", [] {
        int n = 1 << 16, p = n / 3;
        std::vector<Z> a(n), b(n);
        for (int i = 0; i < n; ++i)
            a[i] = i * 3 - n;
        b[p] = 1;
        auto result = bitwiseConvolution<BitwiseOp::Xor>(a, b);
        for (int i = 0; i < n; ++i)
            CHECK(result[i] == a[i ^ p]);
        b[p] = 0;
        b[0] = 1;
        CHECK(bitwiseConvolution<BitwiseOp::Or>(a, b) == a);
        b[0] = 0;
        b[n - 1] = 1;
        CHECK(bitwiseConvolution<BitwiseOp::And>(a, b) == a);
    });
    return 0;
}
