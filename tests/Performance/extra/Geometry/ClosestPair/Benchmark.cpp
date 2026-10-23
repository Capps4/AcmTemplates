#include "../../../../../src/extra/Geometry/ClosestPair/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<Point<long long>> a;
    for (int i=0;i<in.n;++i) { a.emplace_back(in.shape ? 0 : i, in.shape ? 0 : i); }
    return measure(in, "collinear / duplicate closest points", [&]() -> std::uint64_t {
        auto v=closestPair<long long,long long>(a);
        benchmarkCheck(v==(in.shape ? 0 : 2), "known closest pair");
        return std::uint64_t(v);
    });
}
