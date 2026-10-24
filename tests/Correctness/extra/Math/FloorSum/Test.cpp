#include "../../../../../src/extra/Math/FloorSum/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using U = unsigned __int128;
U oracle(std::uint64_t n, std::uint64_t m, std::uint64_t a, std::uint64_t b) {
    U sum = 0;
    for (std::uint64_t i = 0; i < n; ++i)
        sum += (U(a) * i + b) / m;
    return sum;
}

int main() {
    runCase("FloorSum/exhaustive-small", [] {
        for (int n = 0; n <= 12; ++n)
            for (int m = 1; m <= 12; ++m)
                for (int a = 0; a <= 12; ++a)
                    for (int b = 0; b <= 12; ++b)
                        CHECK(floorSum(n, m, a, b) == oracle(n, m, a, b));
    });
    runCase("FloorSum/random-wide-oracle", [] {
        for (int trial = 0; trial < 2000; ++trial) {
            test_context::step = trial;
            auto n = std::uint64_t(randomInt(0, 100));
            auto m = testRng() | 1, a = testRng(), b = testRng();
            CHECK(floorSum(n, m, a, b) == oracle(n, m, a, b));
        }
    });
    runCase("FloorSum/uint64-and-overflow", [] {
        auto hi = std::numeric_limits<std::uint64_t>::max();
        CHECK(floorSum(0, hi, hi, hi) == 0);
        CHECK(floorSum(hi, 1, 0, hi) == U(hi) * hi);
        CHECK(floorSum(hi, 1, 1, 0) == U(hi) * (hi - 1) / 2);
        CHECK(floorSum(hi, 1, 2, 0) == U(hi) * (hi - 1));
        CHECK(floorSum(hi, hi, hi, hi) == U(hi) * (U(hi) + 1) / 2);
        CHECK(floorSum(2, 1, hi, hi) == U(3) * hi);
        CHECK(floorSum(1, hi, hi, hi) == 1);
        bool caught = false;
        try {
            (void)floorSum(hi, 1, hi, hi);
        } catch (const std::overflow_error &) {
            caught = true;
        }
        CHECK(caught);
    });
    return 0;
}
