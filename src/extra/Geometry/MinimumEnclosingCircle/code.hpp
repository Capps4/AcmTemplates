#pragma once
#include "Include.hpp"

using u64 = unsigned long long;

template <class T>
std::optional<Circle<T>> minimumEnclosingCircle(
    std::vector<Point<T>> ps,
    u64 seed = std::chrono::steady_clock::now().time_since_epoch().count(),
    T eps = T(1e-12)) {
    static_assert(!std::is_integral_v<T>, "MinimumEnclosingCircle needs floating coordinates");
    assert(!(eps < 0));
    assert(ps.size() <= std::size_t(std::numeric_limits<int>::max()));
    if (ps.empty())
        return std::nullopt;
    std::mt19937_64 rng(seed);
    std::shuffle(ps.begin(), ps.end(), rng);
    auto diam = [](Point<T> a, Point<T> b) {
        auto o = a + (b - a) / T(2);
        return Circle<T>{o, o.dist(a)};
    };
    auto out = [&](const Circle<T>& c, Point<T> p) {
        return c.o.dist2(p) > c.r * c.r * (T(1) + eps);
    };
    Circle<T> c{ps[0], T(0)};
    for (int i = 0, n = int(ps.size()); i < n; ++i) {
        if (!out(c, ps[i]))
            continue;
        c = {ps[i], T(0)};
        for (int j = 0; j < i; ++j) {
            if (!out(c, ps[j]))
                continue;
            c = diam(ps[i], ps[j]);
            for (int k = 0; k < j; ++k) {
                if (!out(c, ps[k]))
                    continue;
                auto u = ps[j] - ps[i], v = ps[k] - ps[i];
                T d = T(2) * u.cross(v);
                // Use an exact zero test even with a tolerant scalar wrapper.
                if (!(d < 0 or d > 0)) {
                    c = diam(ps[i], ps[j]);
                    for (auto q : {diam(ps[i], ps[k]), diam(ps[j], ps[k])})
                        if (q.r > c.r)
                            c = q;
                } else {
                    auto o = ps[i] + (u.rot90() * v.len2() - v.rot90() * u.len2()) / d;
                    c = {o, o.dist(ps[i])};
                }
            }
        }
    }
    // Cover points accepted within eps without changing the computed center.
    T r2 = 0;
    for (auto p : ps)
        r2 = std::max(r2, c.o.dist2(p));
    c.r = std::sqrt(r2);
    return c;
}
