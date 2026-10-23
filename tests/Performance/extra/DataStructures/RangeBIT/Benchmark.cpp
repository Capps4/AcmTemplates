#include "../../../../../src/extra/DataStructures/RangeBIT/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
return measure(in, "full / unit range updates", [&]() -> std::uint64_t {
        RangeBIT<long long> t(in.n);
        for (int i = 0; i < in.n; ++i) { t.rangeModify(in.shape ? i : 0, in.shape ? i+1 : in.n, 1); }
        auto sum = t.rangeQuery(0,in.n);
        benchmarkCheck(sum == (in.shape ? in.n : 1LL*in.n*in.n), "range sum");
        return std::uint64_t(sum);
    });
}
