#include "../../../../../src/Math/RandomNumberAlgorithm/PollardRho/code.hpp"
#include "../../../../../src/Math/MathPackage/ModuloInteger/code.hpp"
#include "../../../../Support/TestSupport.hpp"
using U = unsigned long long;
std::vector<std::pair<U,int>> trial(U n) {
    std::vector<std::pair<U,int>> result;
    for (U p = 2; p <= n / p; ++p) if (n % p == 0) {
        int count=0;
        do { n /= p; ++count; } while(n % p == 0);
        result.emplace_back(p,count);
    }
    if(n>1)result.emplace_back(n,1);
    return result;
}
int main() {
    for (U seed : {0ULL,1ULL,2ULL,20261001ULL}) {
        PollardRho<U> factorizer(seed);
        CHECK(factorizer.primeFactorize(1).empty());
        for (U n = 2; n < 10000; ++n) {
            auto factor = factorizer.findFactor(n);
            CHECK(factor >= 2 && n % factor == 0);
            CHECK((factor == n) == MillerRabin<U>{}(n));
            CHECK(factorizer.primeFactorize(n) == trial(n));
        }
        std::vector<std::vector<std::pair<U,int>>> expected{
            {{1000000007ULL,1},{1000000009ULL,1}},
            {{2147483647ULL,2}},
            {{3,1},{5,1},{17,1},{257,1},{641,1},{65537,1},{6700417,1}},
            {{2,63}}, {{3,40}}, {{18446744073709551557ULL,1}}
        };
        for (const auto& factors : expected) {
            U n = 1;
            for (auto [p,e] : factors) for (int i=0;i<e;++i) n *= p;
            CHECK(factorizer.primeFactorize(n) == factors);
            if (factors.size() > 1 || factors.front().second > 1) CHECK(factorizer.findFactor(n) < n);
        }
    }
    PollardRho<int> small(7);
    CHECK(small.primeFactorize(2147483647) == (std::vector<std::pair<int,int>>{{2147483647,1}}));
    CHECK(small.primeFactorize(2147483646) == (std::vector<std::pair<int,int>>{{2,1},{3,2},{7,1},{11,1},{31,1},{151,1},{331,1}}));
    ModuloInteger<long long,0>::setMod(101);
    PollardRho<long long> signedRho(3);
    CHECK(signedRho.primeFactorize(1000000016000000063LL) == (std::vector<std::pair<long long,int>>{{1000000007,1},{1000000009,1}}));
    CHECK((ModuloInteger<long long,0>::getMod() == 101));
    std::cout << "PollardRho: four seeds, exhaustive trial, proper factors, large semiprimes, uint64, mod isolation PASS\n";
}
