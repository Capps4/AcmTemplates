#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/Sorting/Discreter/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <climits>

int coreCases() {
    CHECK((std::vector<int>{7} | discreteFrom(std::vector<int>{})) == std::vector<int>{0});
    for (int test = 0; test < 16; ++test) {
        test_context::step = test;
        std::vector<int> a(randomInt(0, 200));
        for (auto& x : a) x = randomInt(-100, 100);
        auto expected = a;
        std::sort(expected.begin(), expected.end());
        expected.erase(std::unique(expected.begin(), expected.end()), expected.end());
        auto disc = expected; // Sorted external basis is the documented input contract.
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
    return 0;
}

#include "../../../../src/Sorting/Discreter/code.hpp"
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
void verifyAdded(const std::vector<int> &a) {
    test_context::describe(a);
    auto b = a;
    std::sort(b.begin(), b.end());
    b.erase(std::unique(b.begin(), b.end()), b.end());
    auto op = discreteFrom(b);
    auto got = a | op;
    for (int i = 0; i < int(a.size()); ++i)
        CHECK(got[i] == int(std::count_if(b.begin(), b.end(), [&](int v) {
                  return v < a[i];
              })));
    CHECK((a | discreteFrom(std::vector<int>(b))) == got);
}

int run() {
    runCase("Discreter/empty", [] {
        verifyAdded({});
    });
    runCase("Discreter/single", [] {
        verifyAdded({-4});
    });
    runCase("Discreter/duplicates", [] {
        verifyAdded({-1, -1, -1});
    });
    runCase("Discreter/descending", [] {
        verifyAdded({0, -1, -2, -3, -4});
    });
    runCase("Discreter/wide-gap", [] {
        verifyAdded({-4, 996});
    });
    runCase("Discreter/interleaved", [] {
        verifyAdded({3, 0, 3, -2, 0, -4});
    });
    return 0;
}
}

int main() {
    runCase("Discreter/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
