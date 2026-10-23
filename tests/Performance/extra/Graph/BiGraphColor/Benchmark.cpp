#include "../../../../../src/extra/Graph/BiGraphColor/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<std::vector<int>> g(in.n);
    for (int x = 1; x < in.n; ++x) {
        int p = in.shape ? 0 : x-1;
        g[x].push_back(p);
        g[p].push_back(x);
    }
    return measure(in, "chain / star bipartite graph", [&]() -> std::uint64_t {
        BiGraphColor a(g);
        benchmarkCheck(a.isBiGraph, "forest is bipartite");
        std::uint64_t sum = 0;
        for (int x : a.col) { sum = benchmarkMix(sum, x); }
        return sum;
    });
}
