#include "../../../../../src/extra/Graph/GeneralMatching/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    std::vector<std::pair<int, int>> ed;
    if (in.shape == 0) {
        int x = 0;
        for (; x + 3 < in.n; x += 4) {
            ed.emplace_back(x, x + 1);
            ed.emplace_back(x + 1, x + 2);
            ed.emplace_back(x + 2, x);
            ed.emplace_back(x + 1, x + 3);
            if (x >= 4) {
                ed.emplace_back(x - 1, x);
            }
        }
        for (; x + 1 < in.n; ++x) {
            ed.emplace_back(x, x + 1);
        }
    } else {
        std::mt19937_64 rng(in.seed);
        for (int x = 0; x + 1 < in.n; ++x) {
            ed.emplace_back(x, x + 1);
        }
        for (int x = 0; x < in.n; ++x) {
            for (int y = x + 2; y < in.n; ++y) {
                if (rng() % 3 == 0) {
                    ed.emplace_back(x, y);
                }
            }
        }
        std::shuffle(ed.begin(), ed.end(), rng);
    }
    std::vector<std::vector<int>> adj(in.n);
    for (auto [x, y] : ed) {
        adj[x].push_back(y);
        adj[y].push_back(x);
    }
    auto run = [&](bool verify) {
        GeneralMatching a(in.n);
        for (auto [x, y] : ed) {
            a.add(x, y);
        }
        int cnt = a.work();
        benchmarkCheck(cnt == in.n / 2, "matching cardinality");
        std::uint64_t sum = cnt;
        int used = 0;
        for (int x = 0; x < in.n; ++x) {
            int y = a.mate[x];
            benchmarkCheck(-1 <= y and y < in.n, "matching vertex range");
            benchmarkCheck(y == -1 or (y != x and a.mate[y] == x), "symmetric matching");
            if (verify and y != -1) {
                ++used;
                benchmarkCheck(std::find(adj[x].begin(), adj[x].end(), y) != adj[x].end(),
                               "matched edge belongs to input");
            }
            sum = benchmarkMix(sum, std::uint64_t(y + 1));
        }
        benchmarkCheck(!verify or used == 2 * cnt, "matching cardinality agrees with witness");
        return sum;
    };
    run(true);
    return measure(in, "Edmonds matching; sparse odd-cycle chain / dense graph", [&] { return run(false); });
}
