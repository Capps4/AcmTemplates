#include "../../../../../src/Math/RandomNumberAlgorithm/PollardRho/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("PollardRho/01-unit", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(1ULL) == (std::vector<std::pair<unsigned long long, int>>{}));
    });
    runCase("PollardRho/02-two", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(2ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{{2ULL, 1}}));
    });
    runCase("PollardRho/03-power2", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(1048576ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{{2ULL, 20}}));
    });
    runCase("PollardRho/04-power3", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(59049ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{{3ULL, 10}}));
    });
    runCase("PollardRho/05-smooth", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(30030ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{
                  {2ULL, 1}, {3ULL, 1}, {5ULL, 1}, {7ULL, 1}, {11ULL, 1}, {13ULL, 1}}));
    });
    runCase("PollardRho/06-square", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(1000006000009ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{{1000003ULL, 2}}));
    });
    runCase("PollardRho/07-close-primes", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(1000036000099ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{{1000003ULL, 1}, {1000033ULL, 1}}));
    });
    runCase("PollardRho/08-large-semiprime", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(1000000016000000063ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{{1000000007ULL, 1},
                                                               {1000000009ULL, 1}}));
    });
    runCase("PollardRho/09-mersenne", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(2305843009213693951ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{{2305843009213693951ULL, 1}}));
    });
    runCase("PollardRho/10-max-u64", [] {
        PollardRho<unsigned long long> f(20261005);
        CHECK(f.primeFactorize(18446744073709551615ULL) ==
              (std::vector<std::pair<unsigned long long, int>>{{3ULL, 1},
                                                               {5ULL, 1},
                                                               {17ULL, 1},
                                                               {257ULL, 1},
                                                               {641ULL, 1},
                                                               {65537ULL, 1},
                                                               {6700417ULL, 1}}));
    });
    return finishCases(10);
}
