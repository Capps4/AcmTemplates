#include "../../../../../src/extra/Graph/OfflineDynamicConnectivity/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    std::vector<std::array<int, 4>> ed;
    std::mt19937_64 rng(in.seed);
    for (int x = 1; x < in.n; ++x) {
        if (in.shape == 0 or in.n < 3) {
            ed.push_back({x, in.n, x - 1, x});
        } else {
            int l = 1 + int(rng() % (in.n - 2));
            int r = l + 1 + int(rng() % (in.n - l - 1));
            ed.push_back({0, l, x - 1, x});
            ed.push_back({r, in.n, x - 1, x});
            ed.push_back({0, l / 2, x - 1, x});
        }
    }
    // For a path, component 0 ends at the first edge with no active copy.
    std::vector<std::vector<std::pair<int, int>>> ev(in.n + 1);
    for (auto [l, r, x, y] : ed) {
        benchmarkCheck(y == x + 1, "path edge input");
        ev[l].emplace_back(y, 1);
        ev[r].emplace_back(y, -1);
    }
    std::vector<int> cnt(in.n), want(in.n);
    std::set<int> off;
    for (int x = 1; x < in.n; ++x) {
        off.insert(x);
    }
    for (int t = 0; t < in.n; ++t) {
        for (auto [x, d] : ev[t]) {
            cnt[x] += d;
            if (cnt[x] == 0) {
                off.insert(x);
            } else {
                off.erase(x);
            }
        }
        want[t] = off.empty() ? in.n : *off.begin();
    }
    return measure(in, "Offline connectivity; growing chain / disconnect and reconnect with overlap", [&] {
        OfflineDynamicConnectivity a(in.n, in.n);
        for (auto [l, r, x, y] : ed) {
            a.add(l, r, x, y);
        }
        std::uint64_t sum = 0;
        a.work([&](int t, const RollbackDSU &d) {
            int siz = d.size(0);
            benchmarkCheck(siz == want[t], "independent active-path components");
            sum = benchmarkMix(sum, siz);
        });
        return sum;
    });
}
