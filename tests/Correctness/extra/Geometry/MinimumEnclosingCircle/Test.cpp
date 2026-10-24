#include "../../../../../src/extra/Geometry/MinimumEnclosingCircle/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using P = Point<long double>;

long double oracle(const std::vector<P>& a) {
    long double best = std::numeric_limits<long double>::infinity();
    auto put = [&](long double x, long double y, long double r2) {
        for (auto p : a) {
            auto dx = p.x - x, dy = p.y - y;
            if (dx * dx + dy * dy > r2 + 1e-15L * std::max(1.L, r2))
                return;
        }
        best = std::min(best, r2);
    };
    for (auto p : a)
        put(p.x, p.y, 0);
    for (int i = 0; i < int(a.size()); ++i)
        for (int j = 0; j < i; ++j) {
            long double x = (a[i].x + a[j].x) / 2, y = (a[i].y + a[j].y) / 2;
            put(x, y, (a[i].x - x) * (a[i].x - x) + (a[i].y - y) * (a[i].y - y));
            for (int k = 0; k < j; ++k) {
                long double b = a[j].x - a[i].x, c = a[j].y - a[i].y;
                long double d = a[k].x - a[i].x, e = a[k].y - a[i].y;
                long double det = b * e - c * d;
                if (det == 0)
                    continue;
                long double f = (b * b + c * c) / 2, g = (d * d + e * e) / 2;
                long double dx = (f * e - c * g) / det, dy = (b * g - f * d) / det;
                put(a[i].x + dx, a[i].y + dy, dx * dx + dy * dy);
            }
        }
    return best;
}

void check(const std::vector<P>& a, u64 seed) {
    auto c = minimumEnclosingCircle(a, seed);
    CHECK(bool(c) == !a.empty());
    if (!c)
        return;
    auto lo = a[0], hi = a[0];
    for (auto p : a) {
        lo.x = std::min(lo.x, p.x);
        lo.y = std::min(lo.y, p.y);
        hi.x = std::max(hi.x, p.x);
        hi.y = std::max(hi.y, p.y);
    }
    auto scale = std::max(hi.x - lo.x, hi.y - lo.y);
    if (scale == 0) {
        CHECK(c->r == 0 and c->o == a[0]);
        return;
    }
    // Normalize the oracle so an absolute epsilon cannot hide a tiny circle.
    std::vector<P> b;
    for (auto p : a)
        b.emplace_back((p.x - lo.x) / scale, (p.y - lo.y) / scale);
    auto expected = oracle(b), r2 = (c->r / scale) * (c->r / scale);
    CHECK(std::abs(r2 - expected) <= 1e-10L * std::max(1.L, expected));
    for (auto p : a) {
        auto dx = (c->o.x - p.x) / scale, dy = (c->o.y - p.y) / scale;
        CHECK(dx * dx + dy * dy <= r2 + 1e-12L * std::max(1.L, r2));
    }
}

int main() {
    runCase("MinimumEnclosingCircle/oracle", [] {
        for (unsigned mask = 0; mask < 512; ++mask) {
            std::vector<P> a;
            for (int i = 0; i < 9; ++i)
                if (mask >> i & 1)
                    a.emplace_back(i / 3 - 1, i % 3 - 1);
            check(a, mask);
        }
        for (int it = 0; it < 100; ++it) {
            std::vector<P> a(randomInt(0, 9));
            for (auto& p : a)
                p = P(randomInt(-12, 12), randomInt(-12, 12));
            check(a, testRng());
        }
    });
    runCase("MinimumEnclosingCircle/degenerate-and-types", [] {
        check({{1, 2}, {1, 2}, {1, 2}}, 0);
        check({{-5, -5}, {-1, -1}, {2, 2}, {3, 3}}, 1);
        check({{0, 0}, {1, 1e-14L}, {2, 0}}, 2);
        check({{1e9L, 1e9L}, {1e9L + 2, 1e9L}, {1e9L, 1e9L + 2}}, 3);
        check({{0, 0}, {1e-9L, 0}}, 7);
        check({{0, 0}, {2e-9L, 0}, {1e-9L, 2e-9L}}, 8);
        check({{0, 0}, {1e-9L, 1e-23L}, {2e-9L, 0}}, 9);
        std::vector<Point<double>> a{{0, 0}, {2, 0}, {0, 2}};
        auto saved = a;
        auto c = minimumEnclosingCircle(a, 4);
        CHECK(a == saved);
        CHECK(c and std::abs(c->r - std::sqrt(2.)) < 1e-12);
        auto d = minimumEnclosingCircle(std::move(a), 4);
        CHECK(d and c->o == d->o and c->r == d->r);
        std::vector<Point<Float>> b{{0, 0}, {2e-9, 0}, {1e-9, std::sqrt(3.) * 1e-9}};
        auto f = minimumEnclosingCircle(b, 5);
        CHECK(f and std::abs(f->r.val() / (2e-9 / std::sqrt(3.)) - 1) < 1e-10);
        std::vector<Point<float>> e{{-1, 0}, {1, 0}, {0, 1}};
        auto g = minimumEnclosingCircle(e, 6, 1e-5f);
        CHECK(g and std::abs(g->r - 1) < 1e-5f);
    });
    runCase("MinimumEnclosingCircle/structure", [] {
        std::vector<P> a{{-1, 0}, {1, 0}};
        for (int i = 0; i < 3000; ++i)
            a.emplace_back((i % 101 - 50) / 100.L, (i % 79 - 39) / 100.L);
        for (u64 seed : {0ULL, 1ULL, 42ULL}) {
            auto c = minimumEnclosingCircle(a, seed);
            CHECK(c and std::abs(c->r - 1) < 1e-12L);
            CHECK(c->o.dist2(P{}) < 1e-20L);
        }
    });
    return 0;
}
