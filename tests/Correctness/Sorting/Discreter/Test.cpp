#include "../../../../src/Sorting/Discreter/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <climits>
int main() {
    CHECK((std::vector<int>{7} | discreteFrom(std::vector<int>{})) == std::vector<int>{0});
    for (int test = 0; test < 1000; ++test) {
        std::vector<int> a(randomInt(0, 200));
        for (auto& x : a) x = randomInt(-100, 100);
        auto expected = a;
        std::sort(expected.begin(), expected.end());
        expected.erase(std::unique(expected.begin(), expected.end()), expected.end());
        auto disc = expected;
        CHECK(disc.size() == expected.size());
        for (int i = 0; i < int(disc.size()); ++i) CHECK(disc[i] == expected[i]);
        for (int x = -101; x <= 101; ++x)
            CHECK(int(std::lower_bound(disc.begin(), disc.end(), x) - disc.begin()) == int(std::lower_bound(expected.begin(), expected.end(), x) - expected.begin()));
        auto ranks = a | discreteFrom(disc);
        for (std::size_t i = 0; i < a.size(); ++i) CHECK(disc[ranks[i]] == a[i]);
    }
    std::vector<long long> limits{LLONG_MIN, -1, 0, LLONG_MAX};
    CHECK((std::vector<long long>{LLONG_MIN, LLONG_MAX, 0, -1, LLONG_MIN} | discreteFrom(limits)) == std::vector<int>({0, 3, 2, 1, 0}));
    std::vector<int> basis{1, 3, 5}, query{0, 1, 2, 5, 7};
    auto borrowed = discreteFrom(basis);
    CHECK((query | borrowed) == std::vector<int>({0, 0, 1, 2, 3}));
    basis = {0, 2, 4, 6};
    CHECK((query | borrowed) == std::vector<int>({0, 1, 1, 3, 4}));
    auto owned = discreteFrom(std::vector<int>{1, 3, 5});
    CHECK((query | owned) == std::vector<int>({0, 0, 1, 2, 3}));
    CHECK((query | owned) == std::vector<int>({0, 0, 1, 2, 3}));
    CHECK((std::vector<bool>{true, false, true} | discreteFrom(std::vector<bool>{false, true})) == std::vector<int>({1, 0, 1}));
    std::cout << "Discreter: sort/lower-bound oracle, empty/extremes, borrowed/owned pipe and bool PASS\n";
}
