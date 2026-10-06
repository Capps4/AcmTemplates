#include "../../../../../src/Math/RandomNumberAlgorithm/MillerRabin/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("MillerRabin/01-zero", [] {
        CHECK(MillerRabin<unsigned long long>{}(0ULL) == false);
    });
    runCase("MillerRabin/02-unit", [] {
        CHECK(MillerRabin<unsigned long long>{}(1ULL) == false);
    });
    runCase("MillerRabin/03-two", [] {
        CHECK(MillerRabin<unsigned long long>{}(2ULL) == true);
    });
    runCase("MillerRabin/04-fermat-prime", [] {
        CHECK(MillerRabin<unsigned long long>{}(65537ULL) == true);
    });
    runCase("MillerRabin/05-carmichael-1", [] {
        CHECK(MillerRabin<unsigned long long>{}(561ULL) == false);
    });
    runCase("MillerRabin/06-carmichael-2", [] {
        CHECK(MillerRabin<unsigned long long>{}(41041ULL) == false);
    });
    runCase("MillerRabin/07-strong-pseudoprime", [] {
        CHECK(MillerRabin<unsigned long long>{}(341550071728321ULL) == false);
    });
    runCase("MillerRabin/08-mersenne-prime", [] {
        CHECK(MillerRabin<unsigned long long>{}(2305843009213693951ULL) == true);
    });
    runCase("MillerRabin/09-unsigned-prime", [] {
        CHECK(MillerRabin<unsigned long long>{}(18446744073709551557ULL) == true);
    });
    runCase("MillerRabin/10-unsigned-max", [] {
        CHECK(MillerRabin<unsigned long long>{}(18446744073709551615ULL) == false);
    });
    return finishCases(10);
}
