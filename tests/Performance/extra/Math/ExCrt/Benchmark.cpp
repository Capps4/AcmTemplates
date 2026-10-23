#include "../../../../../src/extra/Math/ExCrt/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
return measure(in, "compatible / inconsistent congruences", [&]() -> std::uint64_t {
        ExCrt<long long> a;
        for (int i = 0; i < in.n; ++i) {
            a.insert(in.shape and i == in.n - 1 ? 1 : 0, 6);
        }
        auto v = a.get();
        benchmarkCheck(v == (in.shape ? -1 : 0), "CRT consistency");
        return std::uint64_t(v);
    });
}
