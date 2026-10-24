#include "../../../../../src/extra/Graph/DirectedMST/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

using i64 = long long;

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    std::vector<std::tuple<int, int, i64>> ed;
    i64 want = 0;
    if (in.n > 1) {
        ed.emplace_back(0, 1, in.shape == 0 ? in.n : 20);
        want = in.shape == 0 ? in.n : 22 - in.n;
    }
    if (in.shape == 0) {
        for (int x = 2; x < in.n; ++x) {
            ed.emplace_back(x - 1, x, 0);
            ed.emplace_back(x, 1, x - 2);
        }
    } else if (in.n > 1) {
        for (int x = 1; x < in.n; ++x) {
            ed.emplace_back(x, x + 1 == in.n ? 1 : x + 1, -1);
        }
        std::mt19937_64 rng(in.seed);
        for (int i = 0; i < 4 * in.n; ++i) {
            int x = 1 + int(rng() % (in.n - 1)), y = 1 + int(rng() % (in.n - 1));
            ed.emplace_back(x, y, i64(rng() % 100));
        }
        std::shuffle(ed.begin(), ed.end(), rng);
    }
    auto run = [&](bool verify) {
        DirectedMST<i64> a(in.n);
        for (auto [x, y, w] : ed) {
            a.add(x, y, w);
        }
        auto r = a.work(0);
        benchmarkCheck(r and r->cost == want, "optimal arborescence cost");
        benchmarkCheck(int(r->edges.size()) == in.n - 1, "arborescence edge count");
        if (verify) {
            decltype(r->cost) cost = 0;
            std::vector<int> deg(in.n);
            std::vector<std::vector<int>> g(in.n);
            for (int id : r->edges) {
                benchmarkCheck(0 <= id and id < int(ed.size()), "original edge id");
                auto [x, y, w] = ed[id];
                ++deg[y];
                cost += w;
                g[x].push_back(y);
            }
            benchmarkCheck(cost == r->cost and deg[0] == 0, "recovered cost and root indegree");
            for (int x = 1; x < in.n; ++x) {
                benchmarkCheck(deg[x] == 1, "recovered indegree");
            }
            std::vector<char> vis(in.n);
            std::vector<int> bfs{0};
            vis[0] = true;
            for (int i = 0; i < int(bfs.size()); ++i) {
                for (int y : g[bfs[i]]) {
                    if (!vis[y]) {
                        vis[y] = true;
                        bfs.push_back(y);
                    }
                }
            }
            benchmarkCheck(int(bfs.size()) == in.n, "root reaches recovered tree");
        }
        std::uint64_t sum = std::uint64_t(r->cost);
        for (int id : r->edges) {
            sum = benchmarkMix(sum, id);
        }
        return sum;
    };
    run(true);
    return measure(in, "Directed MST; nested contractions / sparse negative cycle", [&] { return run(false); });
}
