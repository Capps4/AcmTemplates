#include "../../../../../src/extra/Math/DiscreteLog/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using i64 = long long;

std::vector<i64> powers(int a, int m) {
    std::vector<i64> first(m, -1);
    int cur = 1 % m;
    i64 i = 0;
    while (first[cur] == -1) {
        first[cur] = i++;
        cur = int(i64(cur) * a % m);
    }
    return first;
}

void exhaustive(int limit) {
    for (int m = 1; m <= limit; ++m) {
        for (int a = 0; a < m; ++a) {
            auto expected = powers(a, m);
            for (int b = 0; b < m; ++b) {
                test_context::input = std::to_string(a) + "," +
                    std::to_string(b) + "," + std::to_string(m);
                CHECK_EQ(discreteLog(a, b, m), expected[b]);
            }
        }
    }
}

void large(int count, bool full) {
    for (int m : {1000000007, 2013265921, INT_MAX}) {
        CHECK_EQ(discreteLog(0, 0, m), 1);
        CHECK_EQ(discreteLog(0, 1, m), 0);
        CHECK_EQ(discreteLog(1, 2, m), -1);
        CHECK_EQ(discreteLog(-1, -1, m), 1);
        for (int trial = 0; trial < count; ++trial) {
            test_context::step = trial;
            int a = randomInt(2, m - 1), k = randomInt(1, 700);
            i64 b = 1;
            for (int i = 0; i < k; ++i)
                b = b * a % m;
            i64 cur = 1, expected = 0;
            while (cur != b) {
                cur = cur * a % m;
                ++expected;
            }
            CHECK_EQ(discreteLog(a, b, m), expected);
        }
    }
    i64 val = 1;
    for (int i = 0; i < 31; ++i) {
        if (full or i == 0 or i == 15 or i == 30)
            CHECK_EQ(discreteLog(2, val, INT_MAX), i);
        val = val * 2 % INT_MAX;
    }
    CHECK_EQ(discreteLog(2, 0, 1 << 30), 30);
    CHECK_EQ(discreteLog(6, 0, 1 << 30), 30);
    CHECK_EQ(discreteLog(2, 3, 1 << 30), -1);
    // 65537-1 is a power of two, so this witness proves that 3 has full order.
    i64 half = 1;
    for (int i = 0; i < 32768; ++i)
        half = half * 3 % 65537;
    CHECK_EQ(half, 65536);
    CHECK_EQ(discreteLog(3, half, 65537), 32768);
    CHECK_EQ(discreteLog(3, 21846, 65537), 65535);
}

int main() {
    runCase("DiscreteLog/small-moduli-exhaustive", [] {
        exhaustive(32);
    });
    runCase("DiscreteLog/signed-input-and-random", [] {
        for (int i = 0; i < 600; ++i) {
            test_context::step = i;
            int m = randomInt(1, 1000);
            i64 a = randomInt(-1000000, 1000000), b = randomInt(-1000000, 1000000);
            auto expected = powers(int((a % m + m) % m), m);
            CHECK_EQ(discreteLog(a, b, m), expected[(b % m + m) % m]);
        }
        for (int m : {1, 2, 7, 64, 999}) {
            for (i64 a : {LLONG_MIN, LLONG_MAX}) {
                auto expected = powers(int((a % m + m) % m), m);
                for (i64 b : {LLONG_MIN, LLONG_MAX})
                    CHECK_EQ(discreteLog(a, b, m), expected[(b % m + m) % m]);
            }
        }
    });
    runCase("DiscreteLog/large-moduli-minimality", [] {
        large(1, false);
    });
    runStressCase("DiscreteLog/full-state-and-large-stress", [] {
        exhaustive(80);
        large(12, true);
    });
    return 0;
}
