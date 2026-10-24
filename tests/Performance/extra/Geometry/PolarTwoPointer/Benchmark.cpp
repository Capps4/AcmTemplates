#include "../../../../../src/extra/Geometry/PolarTwoPointer/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    const std::array<Vec<long long>, 8> dir{{{1, 0}, {1, 1}, {0, 1}, {-1, 1},
                                           {-1, 0}, {-1, -1}, {0, -1}, {1, -1}}};
    std::vector<Vec<long long>> a;
    std::array<int, 9> pre{};
    for (int i = 0; i < in.n; ++i) {
        int d = in.shape ? 0 : i % 8;
        long long k = i % 7 + 1;
        a.emplace_back(dir[d].x * k, dir[d].y * k);
        ++pre[d + 1];
    }
    for (int d = 0; d < 8; ++d) {
        pre[d + 1] += pre[d];
    }
    std::vector<int> want(in.n);
    for (int d = 0; d < 8; ++d) {
        int end = -1;
        for (int k = 4; k < 8; ++k) {
            int e = (d + k) % 8;
            if (pre[e] != pre[e + 1]) {
                end = pre[e];
                break;
            }
        }
        for (int i = pre[d]; i < pre[d + 1]; ++i) {
            want[i] = end == -1 ? i : end;
        }
    }
    std::mt19937_64 rng(in.seed);
    std::shuffle(a.begin(), a.end(), rng);
    return measure(in, "full-circle directions / duplicate directions", [&]() -> std::uint64_t {
        auto v = a;
        auto r = twoPointerWithPolarSorting(v);
        benchmarkCheck(r.size() == want.size(), "endpoint count");
        std::uint64_t sum = 0;
        for (int i = 0; i < in.n; ++i) {
            benchmarkCheck(r[i] == want[i], "independent angular buckets");
            sum = benchmarkMix(sum, r[i]);
        }
        return sum;
    });
}
