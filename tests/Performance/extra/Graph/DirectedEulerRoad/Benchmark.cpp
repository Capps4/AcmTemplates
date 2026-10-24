#include "../../../../../src/extra/Graph/DirectedEulerRoad/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::vector<std::vector<int>> g(in.n);
    for (int x = 1; x < in.n; ++x) {
        g[in.shape ? 0 : x - 1].push_back(x);
        if (in.shape) {
            g[x].push_back(0);
        }
    }
    auto run = [&](bool verify) -> std::uint64_t {
        DirectedEulerRoad a(g);
        benchmarkCheck(a.roadType != "NoSolution", "Euler existence");
        benchmarkCheck(int(a.ans.size()) == (in.shape ? 2 * in.n - 1 : in.n), "Euler path length");
        if (verify) {
            std::vector<int> out(in.n), back(in.n);
            for (int x : a.ans) {
                benchmarkCheck(0 <= x and x < in.n, "Euler vertex range");
            }
            for (int i = 1; i < int(a.ans.size()); ++i) {
                int x = a.ans[i - 1], y = a.ans[i];
                if (in.shape == 0) {
                    benchmarkCheck(y == x + 1, "directed path edge");
                    ++out[y];
                } else {
                    benchmarkCheck(x != y and (x == 0 or y == 0), "star edge");
                    if (x == 0) {
                        ++out[y];
                    } else {
                        ++back[x];
                    }
                }
            }
            for (int x = 1; x < in.n; ++x) {
                benchmarkCheck(out[x] == 1 and back[x] == in.shape, "every directed edge used once");
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
    return measure(in, "directed path / star circuit", [&] { return run(false); });
}
