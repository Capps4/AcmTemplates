#include "../../../../../src/extra/Graph/DirectedEulerRoad/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<std::vector<int>> g(in.n);
    for (int x=1;x<in.n;++x) {
        g[in.shape ? 0 : x-1].push_back(x);
        if (in.shape) { g[x].push_back(0); }
    }
    return measure(in, "directed path / star circuit", [&]() -> std::uint64_t {
        DirectedEulerRoad a(g);
        benchmarkCheck(a.roadType!="NoSolution", "Euler existence");
        std::uint64_t sum=0;
        for (int x:a.ans) { sum=benchmarkMix(sum,x); }
        return sum;
    });
}
