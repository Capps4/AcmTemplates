#include "../../../Support/CaseSupport.hpp"
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
int coreCases() {
    CHECK(multiBag({}, 0) == std::vector<long long>({0}));
    for (int trial = 0; trial < 16; ++trial) {
        test_context::step = trial;
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
    return 0;
}

#include "../../../../src/DynamicProgramming/MultipleBackpacks/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <limits>
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
std::vector<long long> naive(const VecGood &goods, int capacity) {
    std::vector<long long> result(capacity + 1);
    for (auto [value, weight, count] : goods) {
        auto next = result;
        for (int target = 0; target <= capacity; ++target)
            for (int take = 1; take <= count && 1LL * take * weight <= target; ++take)
                next[target] =
                    std::max(next[target], result[target - take * weight] + 1LL * take * value);
        result = std::move(next);
    }
    return result;
}

int run() {
    runCase("MultipleBackpacks/empty", [] {
        VecGood a{};
        CHECK(multiBag(a, 0) == naive(a, 0));
    });
    runCase("MultipleBackpacks/zero-weight", [] {
        VecGood a{{3, 0, 4}};
        CHECK(multiBag(a, 5) == naive(a, 5));
    });
    runCase("MultipleBackpacks/zero-count", [] {
        VecGood a{{7, 1, 0}};
        CHECK(multiBag(a, 3) == naive(a, 3));
    });
    runCase("MultipleBackpacks/negative-value", [] {
        VecGood a{{-5, 2, 4}};
        CHECK(multiBag(a, 9) == naive(a, 9));
    });
    runCase("MultipleBackpacks/heavy", [] {
        VecGood a{{100, 11, 5}};
        CHECK(multiBag(a, 10) == naive(a, 10));
    });
    runCase("MultipleBackpacks/exact-fit", [] {
        VecGood a{{5, 3, 2}};
        CHECK(multiBag(a, 6) == naive(a, 6));
    });
    runCase("MultipleBackpacks/residue", [] {
        VecGood a{{7, 3, 5}, {2, 2, 8}};
        CHECK(multiBag(a, 14) == naive(a, 14));
    });
    runCase("MultipleBackpacks/mixed-zero", [] {
        VecGood a{{3, 0, 4}, {5, 2, 3}};
        CHECK(multiBag(a, 5) == naive(a, 5));
    });
    runCase("MultipleBackpacks/duplicate-types", [] {
        VecGood a{{5, 2, 3}, {5, 2, 4}};
        CHECK(multiBag(a, 15) == naive(a, 15));
    });
    runCase("MultipleBackpacks/count-truncation", [] {
        VecGood a{{9, 1, 1000000}};
        CHECK(multiBag(a, 17) == naive(a, 17));
    });
    return 0;
}
}

int main() {
    runCase("MultipleBackpacks/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
