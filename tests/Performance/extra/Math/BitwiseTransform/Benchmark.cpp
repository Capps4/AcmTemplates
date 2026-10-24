#include "../../../../../src/extra/Math/BitwiseTransform/code.hpp"
#include "../../../../../src/core/Math/Numeric/ModuloInteger/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

std::vector<std::pair<int, Z>> samples(BitwiseOp op, const std::vector<Z> &a,
                                       const std::vector<Z> &b) {
    int n = int(a.size()), all = n - 1;
    std::vector<std::pair<int, Z>> result;
    if (op == BitwiseOp::Xor) {
        for (int k : {0, n / 3, all}) {
            Z value{};
            for (int i = 0; i < n; ++i)
                value += a[i] * b[i ^ k];
            result.push_back({k, value});
        }
        return result;
    }
    // AND is OR after complementing both operand masks and the result mask.
    // Free low bits also cover nonzero coefficients of the sparse workload.
    std::vector<int> masks{0, n / 2, n / 2 | (all & 1), n / 4 | n / 8 | (all & 1)};
    if (op == BitwiseOp::And)
        masks = {0, all & 31, (all & 31) | n / 2, (all & 31) | n / 4 | n / 8};
    for (int mask : masks) {
        Z value{};
        for (int i = mask;; i = (i - 1) & mask) {
            for (int extra = i;; extra = (extra - 1) & i) {
                int j = (mask ^ i) | extra;
                int x = op == BitwiseOp::Or ? i : all ^ i;
                int y = op == BitwiseOp::Or ? j : all ^ j;
                value += a[x] * b[y];
                if (extra == 0)
                    break;
            }
            if (i == 0)
                break;
        }
        result.push_back({op == BitwiseOp::Or ? mask : all ^ mask, value});
    }
    return result;
}

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    int n = 1;
    while (n < input.n)
        n *= 2;
    std::mt19937_64 rng(input.seed);
    std::vector<Z> a(n), b(n);
    for (int i = 0; i < n; ++i) {
        a[i] = input.shape and i % 32 != 0 ? 0 : rng();
        b[i] = input.shape and i % 17 != 0 ? 0 : rng();
    }
    Z expected{};
    for (auto x : a)
        expected += x;
    Z total{};
    for (auto x : b)
        total += x;
    expected *= total;
    auto xorSamples = samples(BitwiseOp::Xor, a, b);
    auto andSamples = samples(BitwiseOp::And, a, b);
    auto orSamples = samples(BitwiseOp::Or, a, b);
    return measure(input, "XOR+AND+OR convolutions: dense / sparse coefficients", [&] {
        std::uint64_t sum = 0;
        auto fold = [&](const std::vector<Z> &c, const std::vector<std::pair<int, Z>> &checks) {
            Z value{};
            for (auto x : c) {
                value += x;
                sum = benchmarkMix(sum, x.val());
            }
            benchmarkCheck(value == expected, "convolution coefficient sum");
            for (auto [k, coefficient] : checks)
                benchmarkCheck(c[k] == coefficient, "independent convolution coefficient");
        };
        fold(bitwiseConvolution<BitwiseOp::Xor>(a, b), xorSamples);
        fold(bitwiseConvolution<BitwiseOp::And>(a, b), andSamples);
        fold(bitwiseConvolution<BitwiseOp::Or>(a, b), orSamples);
        return sum;
    });
}
