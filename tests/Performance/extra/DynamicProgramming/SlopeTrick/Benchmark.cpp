#include "../../../../../src/extra/DynamicProgramming/SlopeTrick/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    std::vector<int> targets(in.n);
    for (auto& target : targets) {
        target = int(rng() % 33) - 16;
    }
    // Projection onto [-16,16] cannot increase either absolute cost or a step of size <= 1.
    // This finite grid therefore contains an optimal path of the unbounded convex DP.
    std::vector<long long> dp(33, 0);
    for (int target : targets) {
        auto next = dp;
        for (int i = 0; i < 33; ++i) {
            if (in.shape) {
                for (int j = std::max(0, i - 1); j <= std::min(32, i + 1); ++j) {
                    next[i] = std::min(next[i], dp[j]);
                }
            }
            next[i] += std::abs(i - 16 - target);
        }
        dp = std::move(next);
    }
    auto expected = *std::min_element(dp.begin(), dp.end());
    return measure(in, "absolute median / bounded velocity convex DP", [&]() {
        SlopeTrick<> function;
        for (int target : targets) {
            if (in.shape) {
                function.shift(-1, 1);
            }
            function.addAbs(target);
        }
        auto [point, value] = function.minimum();
        benchmarkCheck(value == expected, "finite grid independent minimum");
        benchmarkCheck(-16 <= point and point <= 16 and dp[point + 16] == value,
                       "finite grid minimizing point");
        return benchmarkMix(std::uint64_t(value), std::uint64_t(point));
    });
}
