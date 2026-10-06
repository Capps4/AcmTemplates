#include "../../../../src/DynamicProgramming/MultipleBackpacks/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <limits>
std::vector<long long> naive(const VecGood& goods, int capacity) {
    std::vector<long long> result(capacity + 1);
    for (auto [value, weight, count] : goods) {
        auto next = result;
        for (int target = 0; target <= capacity; ++target)
            for (int take = 1; take <= count && 1LL * take * weight <= target; ++take)
                next[target] = std::max(next[target], result[target - take * weight] + 1LL * take * value);
        result = std::move(next);
    }
    return result;
}
int main() {
    CHECK(multiBag({}, 0) == std::vector<long long>({0}));
    for (int trial = 0; trial < 3000; ++trial) {
        int capacity = randomInt(0, 30);
        VecGood goods(randomInt(0, 10));
        for (auto& item : goods) item = {randomInt(-10, 20), randomInt(0, 40), randomInt(0, 7)};
        CHECK(multiBag(goods, capacity) == naive(goods, capacity));
    }
    constexpr int max = std::numeric_limits<int>::max();
    CHECK(multiBag({{max, 0, max}}, 0)[0] == 1LL * max * max);
    CHECK(multiBag({{max, max, max}}, 100) == std::vector<long long>(101));
    CHECK(multiBag({{7, 1, max}}, 100).back() == 700);
    CHECK(multiBag({{3, 0, 4}, {5, 2, 3}}, 5) == std::vector<long long>({12, 12, 17, 17, 22, 22}));
    std::cout << "MultipleBackpacks exhaustive count oracle, zero weight/capacity, negative values, INT_MAX bounds PASS\n";
}
