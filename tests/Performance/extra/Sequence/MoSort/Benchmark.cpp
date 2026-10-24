#include "../../../../../src/extra/Sequence/MoSort/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    int n = in.n, cnt = in.shape ? n * 8 : std::max(1, n / 16);
    std::mt19937_64 rng(in.seed);
    std::vector<int> a(n);
    std::vector<long long> pre(n + 1);
    for (int i = 0; i < n; ++i) {
        a[i] = int(rng() % 97) - 48;
        pre[i + 1] = pre[i] + a[i];
    }
    std::vector<Query> qs;
    for (int i = 0; i < cnt; ++i) {
        int l = int(rng() % (n + 1)), r = int(rng() % (n + 1));
        if (l > r)
            std::swap(l, r);
        qs.emplace_back(l, r, i);
    }
    return measure(in, "Mo sorting and range sums; Q=N/16 / Q=8N", [&]() -> std::uint64_t {
        auto q = qs;
#ifdef MO_SQRT_BASELINE
        int b = int(std::sqrt(2.0 * n)) + 1;
        std::sort(q.begin(), q.end(), [b](const Query& x, const Query& y) {
            int l = x.l / b, r = y.l / b;
            if (l != r)
                return l < r;
            return l % 2 == 0 ? x.r < y.r : x.r > y.r;
        });
#else
        std::sort(q.begin(), q.end(), MoSort(n, cnt));
#endif
        int l = 0, r = 0;
        long long sum = 0;
        std::vector<long long> values(cnt);
        for (auto x : q) {
            while (l > x.l)
                sum += a[--l];
            while (r < x.r)
                sum += a[r++];
            while (l < x.l)
                sum -= a[l++];
            while (r > x.r)
                sum -= a[--r];
            benchmarkCheck(sum == pre[x.r] - pre[x.l], "independent prefix sums");
            values[x.id] = sum;
        }
        std::uint64_t ans = 0;
        for (long long value : values)
            ans = benchmarkMix(ans, std::uint64_t(value));
        return ans;
    });
}
