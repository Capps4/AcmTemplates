#include "../../../../../src/extra/Math/EnumComb/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::vector<std::uint64_t> masks;
    for (int i = 0; i < 32; ++i)
        for (int j = i + 1; j < 32; ++j)
            for (int k = j + 1; k < 32; ++k) {
                std::uint64_t x = (1ULL << i) | (1ULL << j) | (1ULL << k);
                masks.push_back(in.shape ? x ^ 0xffffffffULL : x);
            }
    std::sort(masks.begin(), masks.end());
    int count = std::min(in.n, int(masks.size()));
    std::uint64_t expected = 0;
    for (int i = 0; i < count; ++i)
        expected = benchmarkMix(expected, masks[i]);
    return measure(in, "low / high population combination prefixes", [&]() -> std::uint64_t {
        std::uint64_t sum = 0;
        int cnt = 0;
        for (auto x : EnumComb(32, in.shape ? 29 : 3)) {
            sum = benchmarkMix(sum, x);
            if (++cnt == in.n)
                break;
        }
        benchmarkCheck(cnt == count, "combination prefix count");
        benchmarkCheck(sum == expected, "independently enumerated ordered masks");
        return sum;
    });
}
