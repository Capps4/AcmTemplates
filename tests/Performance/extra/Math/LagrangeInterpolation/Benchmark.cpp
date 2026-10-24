#include "../../../../../src/extra/Math/LagrangeInterpolation/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

Z eval(const std::vector<Z> &a, Z x) {
    Z res = 0;
    for (auto it = a.rbegin(); it != a.rend(); ++it)
        res = res * x + *it;
    return res;
}

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    std::mt19937_64 rng(input.seed);
    std::vector<Z> a(std::min(input.n, 64)), y(input.n), xs;
    for (auto &v : a)
        v = rng();
    for (int i = 0; i < input.n; ++i)
        y[i] = eval(a, Z(i));
    std::uint64_t target = 0;
    for (int i = 0; i < 16; ++i) {
        Z x = input.shape ? Z(-1 - i) : Z(input.n + rng() % (P - input.n));
        xs.push_back(x);
        target = benchmarkMix(target, eval(a, x).val());
    }
    // Keep factorial initialization outside timed repeated evaluation.
    Comb<Z>::shared(input.n - 1);
    return measure(input, "16 consecutive-sample evaluations: random / negative x", [&] {
        std::uint64_t sum = 0;
        for (Z x : xs)
            sum = benchmarkMix(sum, lagrangeInterpolation(y, x).val());
        benchmarkCheck(sum == target, "independent coefficient Horner values");
        return sum;
    });
}
