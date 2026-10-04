#include "Final.hpp"
#include "../TestSupport.hpp"
#include <climits>
int main() {
    Discreter<int> empty;
    CHECK(empty.size() == 0 && empty.rankOf(7) == 0);
    for (int test = 0; test < 1000; ++test) {
        std::vector<int> a(randomInt(0, 200));
        for (auto& x : a) x = randomInt(-100, 100);
        auto expected = a;
        std::sort(expected.begin(), expected.end());
        expected.erase(std::unique(expected.begin(), expected.end()), expected.end());
        Discreter disc(a);
        CHECK(disc.size() == int(expected.size()));
        for (int i = 0; i < disc.size(); ++i) CHECK(disc.at(i) == expected[i]);
        for (int x = -101; x <= 101; ++x)
            CHECK(disc.rankOf(x) == int(std::lower_bound(expected.begin(), expected.end(), x) - expected.begin()));
        auto ranks = a | discreteFrom(disc);
        for (std::size_t i = 0; i < a.size(); ++i) CHECK(disc.at(ranks[i]) == a[i]);
    }
    Discreter limits(std::vector<long long>{LLONG_MIN, LLONG_MAX, 0, -1, LLONG_MIN});
    CHECK(limits.size() == 4 && limits.at(0) == LLONG_MIN);
    std::vector<int> basis{1, 3, 5}, query{0, 1, 2, 5, 7};
    auto borrowed = discreteFrom(basis);
    CHECK((query | borrowed) == std::vector<int>({0, 0, 1, 2, 3}));
    basis = {0, 2, 4, 6};
    CHECK((query | borrowed) == std::vector<int>({0, 1, 1, 3, 4}));
    auto owned = discreteFrom(std::vector<int>{1, 3, 5});
    CHECK((query | owned) == std::vector<int>({0, 0, 1, 2, 3}));
    CHECK((query | owned) == std::vector<int>({0, 0, 1, 2, 3}));
    CHECK((query | discreteFrom(std::vector<int>{5, 3, 1}, std::greater<>{})) == std::vector<int>({3, 2, 2, 0, 0}));
    Discreter bits(std::vector<bool>{true, false, true});
    CHECK(bits.size() == 2 && !bits.at(0) && bits.at(1));
    auto temporary = Discreter<std::string>(std::vector<std::string>{std::string(1000, 'a')}).at(0);
    CHECK(temporary.size() == 1000);
    std::cout << "Discreter: sort/lower-bound oracle, empty/extremes, borrowed/owned pipe and bool PASS\n";
}
