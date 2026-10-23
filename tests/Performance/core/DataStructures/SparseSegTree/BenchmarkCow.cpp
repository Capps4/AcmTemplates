#include "../../../../../src/core/DataStructures/SparseSegTree/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    constexpr long long lim = 1000000000;
    std::mt19937_64 rng(input.seed);
    std::vector<long long> xs(input.n);
    for (auto &x : xs)
        x = static_cast<long long>(rng() % (lim - 16));
    return measure(input, "COW snapshots, additive merge and signed views: narrow / wide ranges",
                   [&]() -> std::uint64_t {
                       using S = SparseSegTree<Info, long long, Tag>;
                       S::clearInit(0, lim, 1);
                       S base;
                       base.modify(0, lim, Tag{1});
                       // Shared branches make merges traverse overlap and retain frozen roots.
                       for (int i = 0; i < 64; ++i)
                           base.modify(xs[i % input.n], xs[i % input.n] + 16, Tag{1});
                       const long long total = base.query(0, lim).sum;
                       const auto old = base.view();
                       std::uint64_t sum = 0;
                       for (int i = 0; i < input.n; ++i) {
                           const long long l = xs[i], r = input.shape ? lim : l + 16;
                           S a = base;
                           a.modify(l, r, Tag{2});
                           const auto delta = a - base;
                           const auto snap = a.view();
                           S b;
                           b.modify(l, r, Tag{3});
                           a.merge(std::move(b));
                           benchmarkCheck(delta.query(l, r).sum == 2 * (r - l), "frozen difference");
                           benchmarkCheck(snap.query(0, lim).sum == total + 2 * (r - l), "frozen snapshot");
                           benchmarkCheck(a.query(0, lim).sum == total + 5 * (r - l), "merged area");
                           sum = benchmarkMix(sum, a.query(l, r).sum);
                       }
                       benchmarkCheck(old.query(0, lim).sum == total, "base snapshot");
                       benchmarkCheck(base.query(0, lim).sum == total, "base remains unchanged");
                       return sum;
                   });
}
