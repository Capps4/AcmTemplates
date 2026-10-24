#include "../../../../../src/extra/DataStructures/RollbackDSU/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    return measure(in, "balanced unions / branch snapshots and rollback", [&]() -> std::uint64_t {
        RollbackDSU d(in.n);
        std::uint64_t sum = 0;
        if (in.shape == 0) {
            for (int len = 1; len < in.n; len *= 2) {
                for (int x = 0; x + len < in.n; x += 2 * len) {
                    benchmarkCheck(d.Union(x, x + len), "new balanced component");
                }
            }
            for (int x = 0; x < in.n; ++x) {
                benchmarkCheck(d.size(x) == in.n, "whole component");
                sum = benchmarkMix(sum, d.size(x));
            }
            d.rollback(0);
        } else {
            for (int first = 1; first < in.n; first += 64) {
                auto s = d.snapshot();
                int last = std::min(first + 64, in.n);
                for (int x = first; x < last; ++x) {
                    benchmarkCheck(d.Union(0, x), "fresh branch");
                    benchmarkCheck(d.size(x) == x - first + 2, "branch size");
                    sum = benchmarkMix(sum, d.size(x));
                }
                d.rollback(s);
            }
        }
        for (int x = 0; x < in.n; ++x) {
            benchmarkCheck(d.size(x) == 1, "rollback restores singleton");
        }
        return sum;
    });
}
