#include "../../../../../src/extra/Graph/UndirectedEulerRoad/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
XorGraph g(in.n);
    for (int x=1;x<in.n;++x) {
        g.add(in.shape ? 0 : x-1,x);
        if (in.shape) { g.add(0,x); }
    }
    return measure(in, "undirected path / parallel star circuit", [&]() -> std::uint64_t {
        UndirectedEulerRoad a(g);
        benchmarkCheck(a.work()!="NoSolution", "Euler existence");
        std::uint64_t sum=0;
        for (int x:a.ans) { sum=benchmarkMix(sum,x); }
        return sum;
    });
}
