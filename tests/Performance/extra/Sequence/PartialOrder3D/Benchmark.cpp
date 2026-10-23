#include "../../../../../src/extra/Sequence/PartialOrder3D/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<std::array<int,3>> a(in.n);
    std::vector<int> w(in.n,1);
    for (int i = 0; i < in.n; ++i) { a[i] = {i, in.shape ? in.n-i : i, i}; }
    return measure(in, "ordered / incomparable 3D points", [&]() -> std::uint64_t {
        auto v = partialOrder<3,long long>(a,w);
        std::uint64_t sum = 0;
        for (int i = 0; i < in.n; ++i) {
            benchmarkCheck(v[i] == (in.shape ? 1 : i+1), "dominance count");
            sum = benchmarkMix(sum,v[i]);
        }
        return sum;
    });
}
