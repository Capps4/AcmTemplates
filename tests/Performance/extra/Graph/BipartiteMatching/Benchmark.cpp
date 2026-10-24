#include "../../../../../src/extra/Graph/BipartiteMatching/code.hpp"
#include "Input.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    auto ed = edges(in);
    return measure(in, "Hopcroft-Karp; long / mixed-length alternating chains", [&]() -> std::uint64_t {
        BipartiteMatching a(in.n, in.n);
        for (auto [x, y] : ed)
            a.add(x, y);
        int cnt = a.work();
        benchmarkCheck(cnt == in.n, "perfect matching size");
        return cnt;
    });
}
