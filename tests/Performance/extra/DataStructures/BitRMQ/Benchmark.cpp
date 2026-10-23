#include "../../../../../src/extra/DataStructures/BitRMQ/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<int> a(in.n);
    for (int i = 0; i < in.n; ++i) { a[i] = in.shape ? 7 : i; }
    return measure(in, "monotone / equal bit RMQ", [&]() -> std::uint64_t {
        BitRMQ<int> t(a);
        std::uint64_t sum = 0;
        for (int i = 0; i < in.n; ++i) {
            int v = t(i,in.n);
            benchmarkCheck(v == (in.shape ? 7 : i), "suffix minimum");
            sum = benchmarkMix(sum,v);
        }
        return sum;
    });
}
