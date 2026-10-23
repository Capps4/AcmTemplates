#include "../../../../../src/extra/Math/RangePrimeCount/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
return measure(in, "prime counting near square boundary", [&]() -> std::uint64_t {
        long long n = 1LL * in.n * in.n + in.shape;
        auto v = cntPrimeLessEqual(n);
        benchmarkCheck(v >= 0 and v <= n, "prime count bounds");
        return std::uint64_t(v);
    });
}
