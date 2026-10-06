#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/RandomNumberAlgorithm/RandomNumber/code.hpp"
#include "../../../../Support/TestSupport.hpp"

std::mt19937_64* engineFromOtherTranslationUnit();
std::uint64_t drawFromOtherTranslationUnit();
int coreCases() {
    CHECK(engineFromOtherTranslationUnit() == &rng);
    rng.seed(20261001);
    std::mt19937_64 reference(20261001);
    for (int i = 0; i < 32; ++i) {
        auto actual = i % 2 ? rng() : drawFromOtherTranslationUnit();
        CHECK(actual == reference());
    }
    rng.seed(17);
    auto value = rng();
    rng.seed(17);
    CHECK(rng() == value);
    std::uniform_int_distribution<int> distribution(-7, 9);
    for (int i = 0; i < 32; ++i) {
        int draw = distribution(rng);
        CHECK(-7 <= draw && draw <= 9);
    }
    std::cout << "RandomNumber engine/reference sequence, seed replay, shared engine across two TUs PASS\n";
    return 0;
}

#include "../../../../../src/Math/RandomNumberAlgorithm/RandomNumber/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {

int run() {
    runCase("RandomNumber/zero-seed", [] {
        rng.seed(0ULL);
        std::mt19937_64 model(0ULL);
        for (int i = 0; i < 32; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/max-seed", [] {
        rng.seed(18446744073709551615ULL);
        std::mt19937_64 model(18446744073709551615ULL);
        for (int i = 0; i < 32; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/high-bit", [] {
        rng.seed(9223372036854775808ULL);
        std::mt19937_64 model(9223372036854775808ULL);
        for (int i = 0; i < 32; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/alternating-bits", [] {
        rng.seed(12297829382473034410ULL);
        std::mt19937_64 model(12297829382473034410ULL);
        for (int i = 0; i < 32; ++i)
            CHECK(rng() == model());
    });
    return 0;
}
}

int main() {
    runCase("RandomNumber/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
