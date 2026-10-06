#include "../../../../../src/Math/MathPackage/ExGcd/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("ExGcd/01-both-zero", [] {
        long long x = 0, y = 0;
        auto g = exgcd(0LL, 0LL, x, y);
        CHECK(g == std::gcd(0LL, 0LL));
        CHECK(__int128(0LL) * x + __int128(0LL) * y == g);
    });
    runCase("ExGcd/02-zero-left", [] {
        long long x = 0, y = 0;
        auto g = exgcd(0LL, 37LL, x, y);
        CHECK(g == std::gcd(0LL, 37LL));
        CHECK(__int128(0LL) * x + __int128(37LL) * y == g);
    });
    runCase("ExGcd/03-zero-right", [] {
        long long x = 0, y = 0;
        auto g = exgcd(41LL, 0LL, x, y);
        CHECK(g == std::gcd(41LL, 0LL));
        CHECK(__int128(41LL) * x + __int128(0LL) * y == g);
    });
    runCase("ExGcd/04-equal", [] {
        long long x = 0, y = 0;
        auto g = exgcd(1001LL, 1001LL, x, y);
        CHECK(g == std::gcd(1001LL, 1001LL));
        CHECK(__int128(1001LL) * x + __int128(1001LL) * y == g);
    });
    runCase("ExGcd/05-coprime", [] {
        long long x = 0, y = 0;
        auto g = exgcd(101LL, 103LL, x, y);
        CHECK(g == std::gcd(101LL, 103LL));
        CHECK(__int128(101LL) * x + __int128(103LL) * y == g);
    });
    runCase("ExGcd/06-divisible", [] {
        long long x = 0, y = 0;
        auto g = exgcd(65536LL, 256LL, x, y);
        CHECK(g == std::gcd(65536LL, 256LL));
        CHECK(__int128(65536LL) * x + __int128(256LL) * y == g);
    });
    runCase("ExGcd/07-fibonacci", [] {
        long long x = 0, y = 0;
        auto g = exgcd(832040LL, 514229LL, x, y);
        CHECK(g == std::gcd(832040LL, 514229LL));
        CHECK(__int128(832040LL) * x + __int128(514229LL) * y == g);
    });
    runCase("ExGcd/08-swapped", [] {
        long long x = 0, y = 0;
        auto g = exgcd(514229LL, 832040LL, x, y);
        CHECK(g == std::gcd(514229LL, 832040LL));
        CHECK(__int128(514229LL) * x + __int128(832040LL) * y == g);
    });
    runCase("ExGcd/09-near-limit", [] {
        long long x = 0, y = 0;
        auto g = exgcd(9223372036854775807LL, 9223372036854775805LL, x, y);
        CHECK(g == std::gcd(9223372036854775807LL, 9223372036854775805LL));
        CHECK(__int128(9223372036854775807LL) * x + __int128(9223372036854775805LL) * y == g);
    });
    runCase("ExGcd/10-large-common-factor", [] {
        long long x = 0, y = 0;
        auto g = exgcd(4000000000LL, 6000000000LL, x, y);
        CHECK(g == std::gcd(4000000000LL, 6000000000LL));
        CHECK(__int128(4000000000LL) * x + __int128(6000000000LL) * y == g);
    });
    return finishCases(10);
}
