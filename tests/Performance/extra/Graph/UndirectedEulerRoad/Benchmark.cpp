#include "../../../../../src/extra/Graph/UndirectedEulerRoad/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    XorGraph g(in.n);
    for (int x = 1; x < in.n; ++x) {
        g.add(in.shape ? 0 : x - 1, x);
        if (in.shape) {
            g.add(0, x);
        }
    }
    auto run = [&](bool verify) -> std::uint64_t {
        UndirectedEulerRoad a(g);
        benchmarkCheck(a.work() != "NoSolution", "Euler existence");
        benchmarkCheck(int(a.ans.size()) == (in.shape ? 2 * in.n - 1 : in.n), "Euler path length");
        if (verify) {
            std::vector<int> cnt(in.n);
            for (int x : a.ans) {
                benchmarkCheck(0 <= x and x < in.n, "Euler vertex range");
            }
            for (int i = 1; i < int(a.ans.size()); ++i) {
                int x = a.ans[i - 1], y = a.ans[i];
                benchmarkCheck(in.shape ? x != y and (x == 0 or y == 0) : std::abs(x - y) == 1,
                               "input edge");
                ++cnt[std::max(x, y)];
            }
            for (int x = 1; x < in.n; ++x) {
                benchmarkCheck(cnt[x] == (in.shape ? 2 : 1), "every undirected edge used once");
            }
            benchmarkCheck(in.shape == 0 or a.ans.front() == a.ans.back(), "closed circuit");
        }
        std::uint64_t sum = 0;
        for (int x : a.ans) {
            sum = benchmarkMix(sum, x);
        }
        return sum;
    };
    run(true);
    return measure(in, "undirected path / parallel star circuit", [&] { return run(false); });
}
