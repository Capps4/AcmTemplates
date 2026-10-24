#include "../../../../../Headers/Headers.hpp"
#include "Input.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    auto g = graph(in);
    return measure(in, "specialized distance sums; chain / star", [&]() -> std::uint64_t {
        std::vector<int> fa(in.n, -1), q{0}, siz(in.n, 1);
        std::vector<long long> ans(in.n);
        for (int i = 0; i < int(q.size()); ++i)
            for (int y : g[q[i]])
                if (y != fa[q[i]]) {
                    fa[y] = q[i];
                    q.push_back(y);
                }
        for (int i = in.n - 1; i > 0; --i) {
            int x = q[i], p = fa[x];
            siz[p] += siz[x];
            ans[p] += ans[x] + siz[x];
        }
        for (int i = 1; i < in.n; ++i) {
            int x = q[i];
            ans[x] = ans[fa[x]] + in.n - 2LL * siz[x];
        }
        std::uint64_t sum = 0;
        for (int x = 0; x < in.n; ++x) {
            benchmarkCheck(ans[x] == expected(in, x), "distance sum formula");
            sum = benchmarkMix(sum, ans[x]);
        }
        return sum;
    });
}
