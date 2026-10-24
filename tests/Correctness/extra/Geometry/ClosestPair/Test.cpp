#include "../../../../../src/extra/Geometry/ClosestPair/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

void check(const std::vector<Point<int>>& a) {
    long long ans = std::numeric_limits<long long>::max();
    for (int i = 0; i < int(a.size()); ++i) {
        for (int j = 0; j < i; ++j) {
            long long x = static_cast<long long>(a[i].x) - a[j].x;
            long long y = static_cast<long long>(a[i].y) - a[j].y;
            ans = std::min(ans, x * x + y * y);
        }
    }
    CHECK((closestPair<int, long long>(a) == ans));
}

void checkFloat(const std::vector<Point<double>>& a) {
    long double want = std::numeric_limits<long double>::max();
    for (int i = 0; i < int(a.size()); ++i) {
        for (int j = 0; j < i; ++j) {
            long double x = static_cast<long double>(a[i].x) - a[j].x;
            long double y = static_cast<long double>(a[i].y) - a[j].y;
            want = std::min(want, x * x + y * y);
        }
    }
    auto got = closestPair<double, long double>(a);
    CHECK(std::isfinite(got));
    CHECK(std::abs(got - want) <= 1e-12L * want);
}

int main() {
    runCase("ClosestPair/oracle", [] {
        check({});
        check({{1, 1}});
        check({{1, 1}, {1, 1}});
        for (int it = 0; it < 40; ++it) {
            std::vector<Point<int>> a(randomInt(1, 40));
            for (auto& p : a) {
                p.x = randomInt(-100, 100);
                p.y = randomInt(-100, 100);
            }
            check(a);
        }
    });
    runCase("ClosestPair/strip-and-boundaries", [] {
        check({{-100, 0}, {-1, 3}, {0, 3}, {100, 0}});
        check({{0, 7}, {0, -2}, {0, 3}, {0, 4}, {0, 30}});
        check({{-1000000000, 0}, {1000000000, 0}});
        std::vector<Point<int>> a;
        for (int x = -3; x <= 3; ++x)
            for (int y = -3; y <= 3; ++y)
                a.emplace_back(x, y);
        std::shuffle(a.begin(), a.end(), testRng);
        check(a);
        a.push_back(a.front());
        check(a);
    });
    runCase("ClosestPair/floating", [] {
        std::vector<Point<double>> a{{0.0, 0.0}, {0.25, 0.0}, {0.0, 0.5}, {8.0, 7.0}};
        CHECK((closestPair<double, long double>(a) == 0.0625L));
        for (double scale : {1e-9, 1e-3, 1., 1e9}) {
            for (int it = 0; it < 4; ++it) {
                std::vector<Point<double>> ps(randomInt(2, 20));
                for (auto &p : ps) {
                    p.x = scale * randomInt(-100, 100) / 16;
                    p.y = scale * randomInt(-100, 100) / 16;
                }
                checkFloat(ps);
            }
        }
        checkFloat({{1e-9, 2e-9}, {1e-9, 2e-9}});
    });
    return 0;
}
