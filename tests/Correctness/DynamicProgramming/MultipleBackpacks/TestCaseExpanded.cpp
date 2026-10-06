#include "../../../../src/DynamicProgramming/MultipleBackpacks/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <limits>
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
#include "../../../Support/CaseSupport.hpp"

int main() {
    runCase("MultipleBackpacks/01-empty", [] {
        VecGood a{};
        CHECK(multiBag(a, 0) == naive(a, 0));
    });
    runCase("MultipleBackpacks/02-zero-weight", [] {
        VecGood a{{3, 0, 4}};
        CHECK(multiBag(a, 5) == naive(a, 5));
    });
    runCase("MultipleBackpacks/03-zero-count", [] {
        VecGood a{{7, 1, 0}};
        CHECK(multiBag(a, 3) == naive(a, 3));
    });
    runCase("MultipleBackpacks/04-negative-value", [] {
        VecGood a{{-5, 2, 4}};
        CHECK(multiBag(a, 9) == naive(a, 9));
    });
    runCase("MultipleBackpacks/05-heavy", [] {
        VecGood a{{100, 11, 5}};
        CHECK(multiBag(a, 10) == naive(a, 10));
    });
    runCase("MultipleBackpacks/06-exact-fit", [] {
        VecGood a{{5, 3, 2}};
        CHECK(multiBag(a, 6) == naive(a, 6));
    });
    runCase("MultipleBackpacks/07-residue", [] {
        VecGood a{{7, 3, 5}, {2, 2, 8}};
        CHECK(multiBag(a, 14) == naive(a, 14));
    });
    runCase("MultipleBackpacks/08-mixed-zero", [] {
        VecGood a{{3, 0, 4}, {5, 2, 3}};
        CHECK(multiBag(a, 5) == naive(a, 5));
    });
    runCase("MultipleBackpacks/09-duplicate-types", [] {
        VecGood a{{5, 2, 3}, {5, 2, 4}};
        CHECK(multiBag(a, 15) == naive(a, 15));
    });
    runCase("MultipleBackpacks/10-count-truncation", [] {
        VecGood a{{9, 1, 1000000}};
        CHECK(multiBag(a, 17) == naive(a, 17));
    });
    return finishCases(10);
}
