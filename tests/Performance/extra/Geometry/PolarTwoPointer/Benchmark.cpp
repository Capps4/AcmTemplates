#include "../../../../../src/extra/Geometry/PolarTwoPointer/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<Vec<long long>> a;
    for (int i=0;i<in.n;++i) { a.emplace_back(in.shape ? 1 : (i%2 ? 1 : -1), in.shape ? 0 : i+1); }
    return measure(in, "polar sweep / duplicate directions", [&]() -> std::uint64_t {
        auto v=a;
        auto r=twoPointerWithPolarSorting(v);
        std::uint64_t sum=0;
        for (int i=0;i<in.n;++i) {
            if (in.shape) { benchmarkCheck(r[i]==i, "equal directions"); }
            sum=benchmarkMix(sum,r[i]);
        }
        return sum;
    });
}
