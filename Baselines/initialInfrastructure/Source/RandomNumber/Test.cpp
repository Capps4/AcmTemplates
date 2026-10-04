#include "Final.hpp"
#include "../TestSupport.hpp"
std::mt19937_64* engineFromOtherTranslationUnit();
std::uint64_t drawFromOtherTranslationUnit();
int main() {
    CHECK(engineFromOtherTranslationUnit() == &rng);
    rng.seed(20261001);
    std::mt19937_64 reference(20261001);
    for (int i = 0; i < 10000; ++i) {
        auto actual = i % 2 ? rng() : drawFromOtherTranslationUnit();
        CHECK(actual == reference());
    }
    rng.seed(17);
    auto value = rng();
    rng.seed(17);
    CHECK(rng() == value);
    std::uniform_int_distribution<int> distribution(-7, 9);
    for (int i = 0; i < 10000; ++i) {
        int draw = distribution(rng);
        CHECK(-7 <= draw && draw <= 9);
    }
    std::cout << "RandomNumber engine/reference sequence, seed replay, shared engine across two TUs PASS\n";
}
