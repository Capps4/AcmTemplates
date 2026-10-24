#include "../../../../../src/core/Graph/Flow/MaxFlow/code.hpp"
#include "Input.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    auto ed = edges(in);
    return measure(in, "Dinic baseline; long / mixed-length alternating chains", [&]() -> std::uint64_t {
        int s = 2 * in.n, t = s + 1;
        Flow<int> a(t + 1);
        for (int x = 0; x < in.n; ++x) {
            a.add(s, x, 1);
            a.add(in.n + x, t, 1);
        }
        for (auto [x, y] : ed)
            a.add(x, in.n + y, 1);
        int cnt = a.work(s, t, in.n);
        benchmarkCheck(cnt == in.n, "perfect matching size");
        return cnt;
    });
}
