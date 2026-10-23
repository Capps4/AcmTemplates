#include "../../../../../src/extra/DataStructures/WeightedDisjointSetUnion/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
return measure(in, "chain / star weighted joins", [&]() -> std::uint64_t {
        WeightedDSU<long long> d(in.n);
        for (int i = 1; i < in.n; ++i) {
            int p = in.shape ? 0 : i-1;
            benchmarkCheck(d.Union(p,i,i-p), "weighted join");
        }
        std::uint64_t sum = 0;
        for (int i = 0; i < in.n; ++i) {
            (void)d.find(i);
            (void)d.find(0);
            benchmarkCheck(d.w[i]-d.w[0] == i, "potential");
            sum = benchmarkMix(sum, d.w[i]);
        }
        return sum;
    });
}
