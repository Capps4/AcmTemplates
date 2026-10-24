#include "../../../../../src/extra/DynamicProgramming/SlopeTrick/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("SlopeTrick/corners", [] {
        SlopeTrick<> f;
        CHECK(f.minimum() == std::make_pair(0LL, 0LL));
        f.addConstant(-7);
        f.addAbs(2);
        CHECK(f.minimum() == std::make_pair(2LL, -7LL));
        f.shift(3);
        CHECK(f.minimum() == std::make_pair(5LL, -7LL));
        f.shift(-2, 4);
        auto [x, y] = f.minimum();
        CHECK(3 <= x and x <= 9 and y == -7);
        f.prefixMin();
        f.suffixMin();
        CHECK(f.minimum() == std::make_pair(0LL, -7LL));
        SlopeTrick<long double> real;
        real.addAbs(0.5L);
        real.addAbs(2.5L);
        auto best = real.minimum();
        CHECK(best.first >= 0.5L and best.first <= 2.5L and best.second == 2);
    });
    runCase("SlopeTrick/grid-oracle", [] {
        constexpr int bound = 512;
        for (int trial = 0; trial < 30; ++trial) {
            SlopeTrick<> f;
            std::vector<long long> value(2 * bound + 1, 0);
            int lo = -bound, hi = bound;
            for (int step = 0; step < 80; ++step) {
                test_context::step = step;
                int op = randomInt(0, 6), a = randomInt(-8, 8);
                if (op <= 2) {
                    if (op == 0) {
                        f.addLeft(a);
                    } else if (op == 1) {
                        f.addRight(a);
                    } else {
                        f.addAbs(a);
                    }
                    for (int x = lo; x <= hi; ++x) {
                        value[x + bound] += op == 0 ? std::max(a - x, 0)
                            : op == 1 ? std::max(x - a, 0) : std::abs(x - a);
                    }
                } else if (op == 3) {
                    f.addConstant(a);
                    for (int x = lo; x <= hi; ++x) {
                        value[x + bound] += a;
                    }
                } else if (op == 4) {
                    f.prefixMin();
                    for (int x = lo + 1; x <= hi; ++x) {
                        value[x + bound] = std::min(value[x + bound], value[x - 1 + bound]);
                    }
                } else if (op == 5) {
                    f.suffixMin();
                    for (int x = hi - 1; x >= lo; --x) {
                        value[x + bound] = std::min(value[x + bound], value[x + 1 + bound]);
                    }
                } else {
                    int l = randomInt(-2, 2), r = randomInt(l, 2);
                    f.shift(l, r);
                    int first = std::max(-bound, lo + r), last = std::min(bound, hi + l);
                    auto next = value;
                    for (int x = first; x <= last; ++x) {
                        long long best = std::numeric_limits<long long>::max();
                        for (int y = x - r; y <= x - l; ++y) {
                            best = std::min(best, value[y + bound]);
                        }
                        next[x + bound] = best;
                    }
                    lo = first;
                    hi = last;
                    value = std::move(next);
                }
                auto [x, y] = f.minimum();
                CHECK(lo <= x and x <= hi);
                CHECK(y == *std::min_element(value.begin() + (lo + bound), value.begin() + (hi + bound + 1)));
                CHECK(value[x + bound] == y);
                if (step % 20 == 19) {
                    int at = (step / 20 % 3 - 1) * 8;
                    auto probe = f;
                    // Rotate the three probe positions; the grid still checks every update.
                    // Larger than either slope: this penalty forces the minimizer to at.
                    for (int count = 0; count <= step + 1; ++count) {
                        probe.addAbs(at);
                    }
                    CHECK(probe.minimum() == std::make_pair(1LL * at, value[at + bound]));
                }
            }
        }
    });
    runCase("SlopeTrick/large-median", [] {
        // Debug libstdc++ validates entire heaps after each update; keep this oracle moderate.
        constexpr int n = 512;
        SlopeTrick<> f;
        std::vector<long long> a;
        for (int i = 0; i < n; ++i) {
            long long x = randomInt(-1000000, 1000000);
            a.push_back(x);
            f.addAbs(x);
        }
        std::sort(a.begin(), a.end());
        long long expected = 0;
        for (auto x : a) {
            expected += std::abs(x - a[n / 2]);
        }
        auto [x, y] = f.minimum();
        CHECK(a[n / 2 - 1] <= x and x <= a[n / 2]);
        CHECK(y == expected);
    });
    return 0;
}
