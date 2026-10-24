#include "../../../../../src/extra/DataStructures/WeightedDisjointSetUnion/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    return measure(in, "balanced / star weighted joins with repeated finds", [&]() -> std::uint64_t {
        WeightedDSU<long long> d(in.n);
        if (in.shape == 0) {
            for (int width = 1; width < in.n; width *= 2) {
                for (int l = 0; l + width < in.n; l += 2 * width) {
                    benchmarkCheck(d.Union(l, l + width, width), "balanced weighted join");
                }
            }
        } else {
            for (int i = 1; i < in.n; ++i) {
                benchmarkCheck(d.Union(0, i, i), "star weighted join");
            }
        }
        std::uint64_t sum = 0;
        int root = d.find(0);
        for (int pass = 0; pass < 2; ++pass) {
            for (int step = 0; step < in.n; ++step) {
                int i = pass == 0 ? in.n - 1 - step : step;
                benchmarkCheck(d.find(i) == root, "weighted component");
                auto value = d.w[i] - d.w[0];
                benchmarkCheck(value == i, "potential before / after compression");
                sum = benchmarkMix(sum, value);
            }
        }
        return sum;
    });
}
