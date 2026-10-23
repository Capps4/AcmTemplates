#include "../../../../../src/extra/DataStructures/RangeMatBIT/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
int n = in.n;
    return measure(in, "whole matrix / row updates", [&]() -> std::uint64_t {
        RangeMatBIT<long long> t(n,n);
        for (int i = 0; i < n; ++i) { t.rangeModify(in.shape ? i : 0,0,in.shape ? i+1 : n,n,1); }
        auto sum = t.rangeQuery(0,0,n,n);
        benchmarkCheck(sum == (in.shape ? 1LL*n*n : 1LL*n*n*n), "rectangle sum");
        return std::uint64_t(sum);
    });
}
