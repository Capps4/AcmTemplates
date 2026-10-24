#include "../../../../../../src/core/DataStructures/RangeQuery/RMQ/code.hpp"
#include "../../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput input(argc, argv);
    std::mt19937_64 random(input.seed);
    std::vector<std::pair<unsigned, unsigned>> a(input.n);
    for (auto& value : a)
        value = {unsigned(random()), unsigned(random())};
    std::vector<std::pair<int, int>> queries;
    for (int i = 0; i < input.n; ++i) {
        int l = int(random() % input.n), r;
        if (input.shape) {
            r = int(random() % input.n);
            if (l > r)
                std::swap(l, r);
            ++r;
        } else {
            r = l + 1 + int(random() % std::min(16, input.n - l));
        }
        queries.emplace_back(l, r);
    }
    return measure(input, "noncommutative fold: short / random intervals", [&]() -> std::uint64_t {
        const RMQ table(a, [](const auto& x, const auto& y) {
            return std::make_pair(x.first, y.second);
        });
        std::uint64_t checksum = 0;
        for (auto [l, r] : queries) {
            auto value = table(l, r);
            benchmarkCheck(value == std::make_pair(a[l].first, a[r - 1].second), "interval endpoints");
            checksum = benchmarkMix(checksum, (std::uint64_t(value.first) << 32) | value.second);
        }
        return checksum;
    });
}
