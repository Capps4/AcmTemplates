#include "../../../../../src/extra/Math/EnumComb/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
return measure(in, "low / high population combinations", [&]() -> std::uint64_t {
        std::uint64_t sum = 0;
        int cnt = 0;
        for (auto x : EnumComb(32, in.shape ? 29 : 3)) {
            sum = benchmarkMix(sum, x);
            if (++cnt == in.n) { break; }
        }
        return sum;
    });
}
