#include "../../../../../src/extra/Math/EnumComb/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("EnumComb/oracle", [] {
        for (int m = 0; m <= 10; ++m) {
            for (int k = 0; k <= m; ++k) {
                std::vector<std::uint64_t> got, want;
                for (auto x : EnumComb(m,k)) { got.push_back(x); }
                for (unsigned x = 0; x < (1U << m); ++x) {
                    if (__builtin_popcount(x) == k) { want.push_back(x); }
                }
                CHECK(got == want);
            }
        }
        int cnt = 0;
        for (auto x : EnumComb(64, 0)) {
            CHECK(x == 0);
            ++cnt;
        }
        CHECK(cnt == 1);
        cnt = 0;
        for (auto x : EnumComb(64,64)) { CHECK(x == ~std::uint64_t(0)); ++cnt; }
        CHECK(cnt == 1);
        cnt = 0;
        for (auto x : EnumComb(64,1)) { CHECK(__builtin_popcountll(x) == 1); ++cnt; }
        CHECK(cnt == 64);
        cnt = 0;
        for (auto x : EnumComb(64, 63)) {
            CHECK(cnt < 64);
            CHECK(x == ~(1ULL << (63 - cnt)));
            ++cnt;
        }
        CHECK(cnt == 64);
    });
    return 0;
}
