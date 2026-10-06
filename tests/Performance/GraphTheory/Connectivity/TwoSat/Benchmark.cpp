#include "../../../../../src/GraphTheory/Connectivity/TwoSat/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    (void)random;
    return measure(input, "implication chain / star construction and solve",
                   [&]() -> std::uint64_t {
                       TwoSat a(n);
                       a.assign(0, true);
                       for (int i = 1; i < n; ++i)
                           a.add(input.shape ? 0 : i - 1, true, i, true);
                       if (!a.work())
                           std::exit(1);
                       return std::count(a.ans.begin(), a.ans.end(), true);
                   });
}
