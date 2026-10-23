#include "../../../../../src/core/DataStructures/SparseSegTree/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    constexpr long long lim = 1000000000;
    std::mt19937_64 rng(input.seed);
    std::vector<long long> xs(input.n);
    for (auto &x : xs)
        x = static_cast<long long>(rng() % (lim - 16));
    return measure(input, "permanent tags: narrow ranges / full domain with narrow queries",
                   [&]() -> std::uint64_t {
                       SparseSegTree<Info, long long, Tag>::clearInit(0, lim, 1);
                       SparseSegTree<Info, long long, Tag> t;
                       std::uint64_t sum = 0;
                       for (int i = 0; i < input.n; ++i) {
                           const auto x = xs[i];
                           t.modify(input.shape ? 0 : x, input.shape ? lim : x + 16, Tag{1});
                           const auto v = t.query(x, x + 16);
                           benchmarkCheck(v.len == 16, "query length");
                           if (input.shape)
                               benchmarkCheck(v.sum == 16LL * (i + 1), "inherited root tag");
                           sum = benchmarkMix(sum, v.sum);
                       }
                       benchmarkCheck(t.query(0, lim).sum == input.n * (input.shape ? lim : 16),
                                      "total added area");
                       return sum;
                   });
}
