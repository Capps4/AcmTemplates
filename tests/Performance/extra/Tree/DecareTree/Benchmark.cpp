#include "../../../../../src/extra/Tree/DecareTree/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<int> a(in.n);
    for (int i = 0; i < in.n; ++i) { a[i] = in.shape ? 0 : i; }
    return measure(in, "increasing / equal Cartesian tree", [&]() -> std::uint64_t {
        DecareTree<int> t(a);
        benchmarkCheck(t.root == (in.shape ? in.n-1 : 0), "Cartesian root");
        std::uint64_t sum = 0;
        for (auto [l,r] : t.t) { sum = benchmarkMix(sum, l+r+2); }
        return sum;
    });
}
