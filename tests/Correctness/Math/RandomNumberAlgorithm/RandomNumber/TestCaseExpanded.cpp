#include "../../../../../src/Math/RandomNumberAlgorithm/RandomNumber/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("RandomNumber/01-zero", [] {
        rng.seed(0ULL);
        std::mt19937_64 model(0ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/02-one", [] {
        rng.seed(1ULL);
        std::mt19937_64 model(1ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/03-max", [] {
        rng.seed(18446744073709551615ULL);
        std::mt19937_64 model(18446744073709551615ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/04-high-bit", [] {
        rng.seed(9223372036854775808ULL);
        std::mt19937_64 model(9223372036854775808ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/05-all-low-bits", [] {
        rng.seed(4294967295ULL);
        std::mt19937_64 model(4294967295ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/06-cross-word", [] {
        rng.seed(4294967296ULL);
        std::mt19937_64 model(4294967296ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/07-alternating", [] {
        rng.seed(12297829382473034410ULL);
        std::mt19937_64 model(12297829382473034410ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/08-reverse-alternating", [] {
        rng.seed(6148914691236517205ULL);
        std::mt19937_64 model(6148914691236517205ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/09-date", [] {
        rng.seed(20261005ULL);
        std::mt19937_64 model(20261005ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    runCase("RandomNumber/10-sparse", [] {
        rng.seed(281474976710657ULL);
        std::mt19937_64 model(281474976710657ULL);
        for (int i = 0; i < 1024; ++i)
            CHECK(rng() == model());
    });
    return finishCases(10);
}
