#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/MathPackage/ExGcd/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <numeric>
#include <climits>


constexpr bool compileTime() {
    long long x = 0, y = 0;
    auto g = exgcd(35LL, 15LL, x, y);
    return g == 5 && 35 * x + 15 * y == 5;
}
static_assert(compileTime());
int coreCases() {
    auto verify = [](long long a, long long b) {
        long long x = 0, y = 0;
        auto g = exgcd(a, b, x, y);
        CHECK(g == std::gcd(a, b));
        CHECK(__int128(a) * x + __int128(b) * y == g);
    };
    verify(0, 0); verify(0, 17); verify(17, 0);
    verify(LLONG_MAX, LLONG_MAX - 1);
    for (int trial = 0; trial < 16; ++trial)
        verify(testRng() % 1000000000000ULL, testRng() % 1000000000000ULL);
    std::cout << "100K Bezout/gcd oracles, zero/extreme inputs and constexpr passed\n";
    return 0;
}

#include "../../../../../src/Math/MathPackage/ExGcd/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {

int run() {
    runCase("ExGcd/both-zero", [] {
        long long x = 0, y = 0;
        auto g = exgcd(0LL, 0LL, x, y);
        CHECK(g == std::gcd(0LL, 0LL));
        CHECK(__int128(0LL) * x + __int128(0LL) * y == g);
    });
    runCase("ExGcd/zero-left", [] {
        long long x = 0, y = 0;
        auto g = exgcd(0LL, 37LL, x, y);
        CHECK(g == std::gcd(0LL, 37LL));
        CHECK(__int128(0LL) * x + __int128(37LL) * y == g);
    });
    runCase("ExGcd/zero-right", [] {
        long long x = 0, y = 0;
        auto g = exgcd(41LL, 0LL, x, y);
        CHECK(g == std::gcd(41LL, 0LL));
        CHECK(__int128(41LL) * x + __int128(0LL) * y == g);
    });
    runCase("ExGcd/equal", [] {
        long long x = 0, y = 0;
        auto g = exgcd(1001LL, 1001LL, x, y);
        CHECK(g == std::gcd(1001LL, 1001LL));
        CHECK(__int128(1001LL) * x + __int128(1001LL) * y == g);
    });
    runCase("ExGcd/coprime", [] {
        long long x = 0, y = 0;
        auto g = exgcd(101LL, 103LL, x, y);
        CHECK(g == std::gcd(101LL, 103LL));
        CHECK(__int128(101LL) * x + __int128(103LL) * y == g);
    });
    runCase("ExGcd/divisible", [] {
        long long x = 0, y = 0;
        auto g = exgcd(65536LL, 256LL, x, y);
        CHECK(g == std::gcd(65536LL, 256LL));
        CHECK(__int128(65536LL) * x + __int128(256LL) * y == g);
    });
    runCase("ExGcd/fibonacci", [] {
        long long x = 0, y = 0;
        auto g = exgcd(832040LL, 514229LL, x, y);
        CHECK(g == std::gcd(832040LL, 514229LL));
        CHECK(__int128(832040LL) * x + __int128(514229LL) * y == g);
    });
    runCase("ExGcd/swapped", [] {
        long long x = 0, y = 0;
        auto g = exgcd(514229LL, 832040LL, x, y);
        CHECK(g == std::gcd(514229LL, 832040LL));
        CHECK(__int128(514229LL) * x + __int128(832040LL) * y == g);
    });
    runCase("ExGcd/near-limit", [] {
        long long x = 0, y = 0;
        auto g = exgcd(9223372036854775807LL, 9223372036854775805LL, x, y);
        CHECK(g == std::gcd(9223372036854775807LL, 9223372036854775805LL));
        CHECK(__int128(9223372036854775807LL) * x + __int128(9223372036854775805LL) * y == g);
    });
    runCase("ExGcd/large-common-factor", [] {
        long long x = 0, y = 0;
        auto g = exgcd(4000000000LL, 6000000000LL, x, y);
        CHECK(g == std::gcd(4000000000LL, 6000000000LL));
        CHECK(__int128(4000000000LL) * x + __int128(6000000000LL) * y == g);
    });
    return 0;
}
}

int main() {
    runCase("ExGcd/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
