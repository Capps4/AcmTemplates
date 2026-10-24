#include "../../../../../src/extra/Geometry/PolarTwoPointer/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using V = Vec<long long>;

long double angle(const V& p) {
    auto a = std::atan2(static_cast<long double>(p.y), static_cast<long double>(p.x));
    return a < 0 ? a + 2 * std::acos(-1.L) : a;
}

void check(const std::vector<V>& src) {
    const auto pi = std::acos(-1.L);
    std::vector<long double> ref;
    std::vector<std::pair<long long, long long>> want, got;
    for (auto p : src) {
        ref.push_back(angle(p));
        want.emplace_back(p.x, p.y);
    }
    std::sort(ref.begin(), ref.end());
    std::sort(want.begin(), want.end());
    auto a = src;
    auto ans = twoPointerWithPolarSorting(a);
    CHECK(ans.size() == src.size());
    CHECK(a.size() == src.size());
    for (int i = 0; i < int(a.size()); ++i) {
        got.emplace_back(a[i].x, a[i].y);
        CHECK(std::abs(angle(a[i]) - ref[i]) < 1e-15L);
        int k = 1;
        for (; k < int(a.size()); ++k) {
            auto d = ref[(i + k) % a.size()] - ref[i];
            if (d < -1e-15L) {
                d += 2 * pi;
            }
            if (d >= pi - 1e-15L) {
                break;
            }
        }
        CHECK(ans[i] == (i + k) % int(a.size()));
    }
    std::sort(got.begin(), got.end());
    CHECK(got == want);
}

int main() {
    runCase("PolarTwoPointer/oracle", [] {
        check({});
        check({{1, 0}});
        check({{3, 0}, {1, 0}, {2, 0}});
        check({{0, -1}, {2, 0}, {-1, 0}, {0, 1}, {1, 0}, {0, -2}});
        check({{3, -1}, {3, 1}, {-3, 1}, {-3, -1}, {6, 2}, {-6, -2}});
        std::vector<V> ties, stable;
        for (int i = 1; i <= 12; ++i) {
            ties.emplace_back(0, i);
            ties.emplace_back(i, 0);
            ties.emplace_back(-i, 0);
        }
        for (auto d : std::vector<V>{{1, 0}, {0, 1}, {-1, 0}}) {
            for (int i = 1; i <= 12; ++i) {
                stable.emplace_back(d.x * i, d.y * i);
            }
        }
        twoPointerWithPolarSorting(ties);
        CHECK(ties == stable);
        for (int it = 0; it < 30; ++it) {
            std::vector<V> a;
            for (int i = 0; i < 30; ++i) {
                int x = 0, y = 0;
                while (x == 0 and y == 0) {
                    x = randomInt(-3, 3);
                    y = randomInt(-3, 3);
                }
                a.emplace_back(x, y);
            }
            check(a);
        }
    });
}
