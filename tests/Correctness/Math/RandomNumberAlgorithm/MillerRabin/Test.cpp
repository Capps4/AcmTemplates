#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/RandomNumberAlgorithm/MillerRabin/code.hpp"
#include "../../../../../src/Math/MathPackage/ModuloInteger/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <limits>
#include <vector>

constexpr MillerRabin<int> small;
static_assert(small(2) && small(97) && !small(1) && !small(341));
static_assert(isPrime(2305843009213693951LL));
static_assert(!isPrime(3825123056546413051LL));
// Independent additive modular multiplication, avoids 128-bit multiplication.
using U = unsigned long long;
U add(U x, U y, U n) { return x >= n - y ? x - (n - y) : x + y; }
U multiply(U a, U b, U n) {
    U result = 0;
    for (; b; b >>= 1) { if (b & 1) result = add(result, a, n); a = add(a, a, n); }
    return result;
}
bool reference(U n) {
    if (n < 2) return false;
    if (!(n & 1)) return n == 2;
    U odd = n - 1; int twos = 0;
    while (!(odd & 1)) { odd >>= 1; ++twos; }
    // Same proven bases; arithmetic and control flow are independent.
    for (U base : {2ULL,325ULL,9375ULL,28178ULL,450775ULL,9780504ULL,1795265022ULL}) {
        base %= n; if (!base) continue;
        U exponent = odd, x = 1;
        while (exponent) {
            if (exponent & 1) x = multiply(x, base, n);
            exponent >>= 1;
            if (exponent) base = multiply(base, base, n);
        }
        bool passed = x == 1;
        for (int i = 0; i < twos; ++i) {
            passed |= x == n - 1;
            x = multiply(x, x, n);
        }
        if (!passed) return false;
    }
    return true;
}
int coreCases() {
    const int limit = 256;
    std::vector<bool> prime(limit + 1, true); prime[0] = prime[1] = false;
    for (int i = 2; i <= limit / i; ++i) if (prime[i])
        for (int j = i * i; j <= limit; j += i) prime[j] = false;
    MillerRabin<unsigned int> unsignedSmall;
    for (int n = 0; n <= limit; ++n) {
        CHECK(small(n) == prime[n]); CHECK(isPrime(n) == prime[n]);
        CHECK(unsignedSmall(n) == prime[n]);
    }
    CHECK(!isPrime(-1)); CHECK(!isPrime(std::numeric_limits<long long>::min()));
    for (long long n : {341LL,561LL,1105LL,1729LL,3215031751LL,3825123056546413051LL,9223372036854775807LL}) CHECK(!isPrime(n));
    MillerRabin<U> unsignedLarge;
    CHECK(unsignedLarge(18446744073709551557ULL));
    CHECK(!unsignedLarge(std::numeric_limits<U>::max()));
    CHECK(unsignedSmall(4294967291U)); CHECK(!unsignedSmall(4294967295U));
    for (int i = 0; i < 16; ++i) {
        auto value = testRng();
        CHECK(unsignedLarge(value) == reference(value));
    }
    using Dynamic = ModuloInteger<long long,0>;
    Dynamic::setMod(1000000007);
    Dynamic existing = 1000000006;
    CHECK(isPrime(2305843009213693951LL));
    CHECK(Dynamic::getMod() == 1000000007 && existing + 2 == Dynamic(1));
    std::cout << "MillerRabin: exhaustive sieve, pseudoprimes, independent uint64 arithmetic, constexpr, mod isolation PASS\n";
    return 0;
}

#include "../../../../../src/Math/RandomNumberAlgorithm/MillerRabin/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {

int run() {
    runCase("MillerRabin/fermat-prime", [] {
        CHECK(MillerRabin<unsigned long long>{}(65537ULL) == true);
    });
    runCase("MillerRabin/carmichael-1", [] {
        CHECK(MillerRabin<unsigned long long>{}(561ULL) == false);
    });
    runCase("MillerRabin/carmichael-2", [] {
        CHECK(MillerRabin<unsigned long long>{}(41041ULL) == false);
    });
    runCase("MillerRabin/strong-pseudoprime", [] {
        CHECK(MillerRabin<unsigned long long>{}(341550071728321ULL) == false);
    });
    runCase("MillerRabin/mersenne-prime", [] {
        CHECK(MillerRabin<unsigned long long>{}(2305843009213693951ULL) == true);
    });
    runCase("MillerRabin/unsigned-prime", [] {
        CHECK(MillerRabin<unsigned long long>{}(18446744073709551557ULL) == true);
    });
    runCase("MillerRabin/unsigned-max", [] {
        CHECK(MillerRabin<unsigned long long>{}(18446744073709551615ULL) == false);
    });
    return 0;
}
}

int main() {
    runCase("MillerRabin/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
