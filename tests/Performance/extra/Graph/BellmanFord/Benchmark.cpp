#include "../../../../../src/extra/Graph/BellmanFord/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

using i64 = long long;

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    std::vector<std::tuple<int, int, i64>> ed;
    std::vector<i64> pot(in.n);
    std::mt19937_64 rng(in.seed);
    for (int x = 0; x < in.n; ++x) {
        pot[x] = in.shape == 0 ? -i64(x) : i64(rng() % 20001) - 10000;
    }
    for (int x = in.n - 1; x > 0; --x) {
        ed.emplace_back(x - 1, x, pot[x] - pot[x - 1]);
    }
    if (in.shape == 1) {
        for (int i = 0; i < 4 * in.n; ++i) {
            int x = int(rng() % in.n), y = int(rng() % in.n);
            ed.emplace_back(x, y, pot[y] - pot[x] + i64(rng() % 100));
        }
        std::shuffle(ed.begin(), ed.end(), rng);
    }
    return measure(in, "Bellman-Ford; reversed chain / potential-weighted sparse graph", [&] {
        BellmanFord<i64> a(in.n);
        for (auto [x, y, w] : ed) {
            a.add(x, y, w);
        }
        auto r = a.work(0);
        std::uint64_t sum = 0;
        benchmarkCheck(r.cycle.empty(), "no negative cycle");
        for (int x = 0; x < in.n; ++x) {
            benchmarkCheck(r.state[x] == BellmanFord<i64>::Finite and
                           r.dis[x] == pot[x] - pot[0], "potential distance");
            sum = benchmarkMix(sum, std::uint64_t(r.dis[x]));
        }
        return sum;
    });
}
