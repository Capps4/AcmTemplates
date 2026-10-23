#include "../../../../../src/extra/DataStructures/MatBIT/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
int n = in.n;
    return measure(in, "diagonal / repeated matrix updates", [&]() -> std::uint64_t {
        MatBIT<long long> t(n,n);
        for (int i = 0; i < n; ++i) { t.modify(in.shape ? 0 : i,i,1); }
        auto sum = t.rangeQuery(0,0,n,n);
        benchmarkCheck(sum == n, "matrix sum");
        return std::uint64_t(sum);
    });
}
