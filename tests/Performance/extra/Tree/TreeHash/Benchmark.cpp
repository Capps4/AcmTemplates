#include "../../../../../src/extra/Tree/TreeHash/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
TreeHash h(in.seed);
    return measure(in, "small / high-bit subtree hashes", [&]() -> std::uint64_t {
        std::uint64_t sum = 0;
        for (int i = 0; i < in.n; ++i) { sum = benchmarkMix(sum,h(in.shape ? std::uint64_t(i)<<32 : i)); }
        return sum;
    });
}
