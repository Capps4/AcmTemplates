#include "../../../../../src/extra/DataStructures/SparseMatBIT/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    constexpr long long gap = 1000000000LL;
    std::vector<std::pair<long long, long long>> points;
    for (int i = 0; i < in.n; ++i) {
        points.push_back({i * gap, (in.shape ? in.n - 1 - i : i) * gap});
    }
    return measure(in, "sparse diagonal / reversed ordinate point sums", [&]() -> std::uint64_t {
        SparseMatBIT<long long, long long> t(points);
        for (int i = 0; i < in.n; ++i) {
            t.modify(points[i].first, points[i].second, i + 1);
        }
        std::uint64_t sum = 0;
        for (int i = 0; i < in.n; ++i) {
            int l = in.shape ? in.n - 1 - i : 0;
            long long expected = l <= i ? 1LL * (l + i + 2) * (i - l + 1) / 2 : 0;
            auto value = t.prefixQuery((i + 1) * gap, (i + 1) * gap);
            benchmarkCheck(value == expected, "triangular prefix sum");
            sum = benchmarkMix(sum, value);
        }
        return sum;
    });
}
