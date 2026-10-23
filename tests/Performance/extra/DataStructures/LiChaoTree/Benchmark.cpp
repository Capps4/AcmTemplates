#include "../../../../../src/extra/DataStructures/LiChaoTree/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    struct Line {
        using Key = long long;
        using Value = long long;
        long long k, b;
        long long operator[](long long x) const { return k * x + b; }
    };
    std::vector<long long> xs(in.n);
    std::iota(xs.begin(), xs.end(), 0LL);
    // 2*i*x-i*i = x*x-(x-i)^2, so line i=x wins at each registered integer x.
    std::uint64_t expected = 0;
    for (int x = 0; x < in.n; ++x) {
        auto value = in.shape ? 1LL * x * x : in.n - 1;
        expected = benchmarkMix(expected, value);
    }
    return measure(in, "parallel / in-domain crossing Li Chao lines", [&]() -> std::uint64_t {
        LiChaoTree<Line> t(xs);
        for (int i = 0; i < in.n; ++i) {
            t.insert(0, in.n - 1, {in.shape ? 2LL * i : 0, in.shape ? -1LL * i * i : i});
        }
        std::uint64_t sum = 0;
        for (int i = 0; i < in.n; ++i) {
            auto v = t.query(i);
            sum = benchmarkMix(sum, v);
        }
        benchmarkCheck(sum == expected, "known line envelope checksum");
        return sum;
    });
}
