#include "Final.hpp"
#include "../TestSupport.hpp"
#include <numeric>
#include <climits>

constexpr bool compileTime() {
    long long x = 0, y = 0;
    auto g = exgcd(35LL, 15LL, x, y);
    return g == 5 && 35 * x + 15 * y == 5;
}
static_assert(compileTime());
int main() {
    auto verify = [](long long a, long long b) {
        long long x = 0, y = 0;
        auto g = exgcd(a, b, x, y);
        CHECK(g == std::gcd(a, b));
        CHECK(__int128(a) * x + __int128(b) * y == g);
    };
    verify(0, 0); verify(0, 17); verify(17, 0);
    verify(LLONG_MAX, LLONG_MAX - 1);
    for (int trial = 0; trial < 100000; ++trial)
        verify(testRng() % 1000000000000ULL, testRng() % 1000000000000ULL);
    std::cout << "100K Bezout/gcd oracles, zero/extreme inputs and constexpr passed\n";
}
