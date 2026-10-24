#include "../../../../../src/extra/Math/ModSqrt/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using i64 = long long;

bool prime(int p) {
    if (p < 2)
        return false;
    for (int d = 2; d * d <= p; ++d) {
        if (p % d == 0)
            return false;
    }
    return true;
}

int main() {
    runCase("ModSqrt/small-primes-exhaustive", [] {
        for (int p = 2; p <= 257; ++p) {
            if (not prime(p))
                continue;
            std::vector<int> expected(p, -1);
            for (int x = p - 1; x >= 0; --x)
                expected[x * x % p] = x;
            for (int a = -p; a <= 2 * p; ++a) {
                test_context::input = std::to_string(a) + "," + std::to_string(p);
                CHECK_EQ(modSqrt(a, p), expected[(a % p + p) % p]);
            }
        }
    });
    runCase("ModSqrt/large-primes-and-extremes", [] {
        for (int p : {2, 17, 257, 65537, 998244353, 1000000007, 2013265921, INT_MAX}) {
            for (int i = 0; i < 200; ++i) {
                test_context::step = i;
                i64 x = randomInt(0, p - 1);
                i64 a = x * x % p;
                int expected = int(std::min(x, p - x));
                CHECK_EQ(modSqrt(a, p), expected);
                CHECK_EQ(modSqrt(a - p, p), expected);
                CHECK_EQ(modSqrt(a + p, p), expected);
            }
            CHECK_EQ(modSqrt(0, p), 0);
            CHECK_EQ(modSqrt(p, p), 0);
            CHECK_EQ(modSqrt(-i64(p), p), 0);
        }
        CHECK_EQ(modSqrt(INT_MAX - 1LL, INT_MAX), -1);
        for (int p : {2, 3, 5, 17, 257}) {
            for (i64 a : {LLONG_MIN, LLONG_MAX}) {
                i64 val = (a % p + p) % p;
                int expected = -1;
                for (int x = 0; x < p; ++x) {
                    if (x * x % p == val) {
                        expected = x;
                        break;
                    }
                }
                CHECK_EQ(modSqrt(a, p), expected);
            }
        }
    });
    return 0;
}
