#include "../../../../../src/extra/Tree/TreeDiameter/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<std::vector<int>> g(in.n);
    for (int x = 1; x < in.n; ++x) {
        int p = in.shape ? 0 : x-1;
        g[x].push_back(p); g[p].push_back(x);
    }
    return measure(in, "chain / star diameter", [&]() -> std::uint64_t {
        TreeDiameter<int> t(g);
        benchmarkCheck(t.diameter == (in.shape ? std::min(2,in.n-1) : in.n-1), "known diameter");
        return std::uint64_t(t.diameter);
    });
}
