#pragma once
#include "PolygonConvex.hpp"

// SNIPPET BEGIN
namespace _geo2 {

template <class T>
struct Circle {
    Point<T> o;
    T r;
    Circle(Point<T> o, T r) : o(o), r(r) { assert(r >= 0); }
    static Circle circum(Point<T> a, Point<T> b, Point<T> c) {
        static_assert(!std::is_integral_v<T>, "circumcircle needs floating coordinates");
        auto u = b - a, v = c - a;
        T d = u.cross(v) * T(2);
        assert(d != 0);
        auto o = a + (v.rot90() * u.len2() - u.rot90() * v.len2()) / (-d);
        return {o, o.dist(a)};
    }
    auto area() const { return std::acos(T(-1)) * r * r; }
    auto perimeter() const { return std::acos(T(-1)) * r * T(2); }
    Location loc(Point<T> p) const {
        T d = o.dist2(p), r2 = r * r;
        return d == r2 ? Location::ON : d < r2 ? Location::IN : Location::OUT;
    }
};

template <class T>
bool inter(const Circle<T> &c, const Line<T> &l, std::nullptr_t) {
    auto d = l.eval(c.o);
    if constexpr (std::is_integral_v<T>)
        return __int128_t(d) * d <= __int128_t(c.r) * c.r * l.v.len2();
    else
        return d * d <= c.r * c.r * l.v.len2();
}

template <class T>
bool inter(const Circle<T> &c, const Seg<T> &s, std::nullptr_t) {
    T r2 = c.r * c.r;
    if (!(std::max(c.o.dist2(s.a), c.o.dist2(s.b)) >= r2)) return false;
    if (s.a == s.b || s.vec().dot(c.o - s.a) <= 0) return c.o.dist2(s.a) <= r2;
    if (s.vec().dot(c.o - s.b) >= 0) return c.o.dist2(s.b) <= r2;
    return inter(c, s.line(), nullptr);
}

template <class T>
Hit<T> inter(const Circle<T> &c, const Line<T> &l) {
    static_assert(!std::is_integral_v<T>, "boundary coordinates need floating T");
    auto p = near(c.o, l).second;
    T h2 = c.r * c.r - c.o.dist2(p);
    if (h2 == 0) return {HitKind::ONE, {p, {}}};
    if (h2 < 0) return {};
    auto v = l.v * std::sqrt(h2 / l.v.len2());
    return {HitKind::TWO, {p - v, p + v}};
}

template <class T>
Hit<T> inter(const Circle<T> &c, const Seg<T> &s) {
    static_assert(!std::is_integral_v<T>, "intersection coordinates need floating T");
    if (s.a == s.b) return c.loc(s.a) == Location::ON ? Hit<T>{HitKind::ONE, {s.a, {}}} : Hit<T>{};
    auto v = s.vec(), u = s.a - c.o;
    T a = v.len2(), b = v.dot(u), d = u.len2() - c.r * c.r;
    std::array<T, 2> roots;
    // Preserve exact endpoint contacts and test the segment parameter directly.
    if (d == 0) roots = {T(0), -T(2) * b / a};
    else if (c.loc(s.b) == Location::ON) roots = {d / a, T(1)};
    else {
        T cross = v.cross(u);
        T det = Vec<T>{a, cross}.cross({cross, c.r * c.r});
        if (det != 0 && det < 0) return {};
        T h = std::sqrt(std::max(T(0), det));
        T q = -b - (b < 0 ? -h : h);
        roots = q == 0 ? std::array<T, 2>{-b / a, -b / a} : std::array<T, 2>{q / a, d / q};
    }
    if (roots[1] < roots[0]) std::swap(roots[0], roots[1]);
    Hit<T> ans;
    int count = 0;
    for (T t : roots) if (t >= 0 && t <= 1) {
        auto p = t == 0 ? s.a : t == 1 ? s.b : s.a + v * t;
        if (!count || p != ans.ps[0]) ans.ps[count++] = p;
    }
    ans.kind = count == 2 ? HitKind::TWO : count == 1 ? HitKind::ONE : HitKind::NONE;
    return ans;
}


enum class CircleRelation { SEPARATE, EXTERNAL_TANGENT, SECANT, INTERNAL_TANGENT, CONTAINED, COINCIDENT };
template <class T>
CircleRelation relation(const Circle<T> &a, const Circle<T> &b) {
    T d = a.o.dist2(b.o), sum = (a.r + b.r) * (a.r + b.r), diff = (a.r - b.r) * (a.r - b.r);
    if (a.o == b.o && a.r == b.r) return CircleRelation::COINCIDENT;
    if (d == sum) return CircleRelation::EXTERNAL_TANGENT;
    if (d > sum) return CircleRelation::SEPARATE;
    if (d == diff) return CircleRelation::INTERNAL_TANGENT;
    return d < diff ? CircleRelation::CONTAINED : CircleRelation::SECANT;
}

template <class T>
Hit<T> inter(const Circle<T> &a, const Circle<T> &b) {
    static_assert(!std::is_integral_v<T>, "boundary coordinates need floating T");
    auto rel = relation(a, b);
    if (rel == CircleRelation::COINCIDENT) return a.r == 0 ? Hit<T>{HitKind::ONE, {a.o, {}}} : Hit<T>{HitKind::CO, {}};
    if (rel == CircleRelation::SEPARATE || rel == CircleRelation::CONTAINED) return {};
    auto v = b.o - a.o;
    T d2 = v.len2(), x = (d2 + a.r * a.r - b.r * b.r) / (T(2) * d2);
    auto p = a.o + v * x;
    T h2 = a.r * a.r - d2 * x * x;
    if (rel != CircleRelation::SECANT || h2 == 0) return {HitKind::ONE, {p, {}}};
    auto w = v.rot90() * std::sqrt(std::max(T(0), h2) / d2);
    return {HitKind::TWO, {p - w, p + w}};
}

template <class T>
bool inter(const Circle<T> &a, const Circle<T> &b, std::nullptr_t) {
    T d2 = a.o.dist2(b.o);
    return (a.r - b.r) * (a.r - b.r) <= d2 && d2 <= (a.r + b.r) * (a.r + b.r);
}

template <class T>
auto intersectionArea(const Circle<T> &a, const Circle<T> &b) {
    static_assert(!std::is_integral_v<T>, "intersection area needs floating T");
    auto rel = relation(a, b);
    if (rel == CircleRelation::SEPARATE || rel == CircleRelation::EXTERNAL_TANGENT) return T(0);
    if (rel != CircleRelation::SECANT) return std::min(a.area(), b.area());
    T d = a.o.dist(b.o);
    T x = std::acos(std::clamp((d * d + a.r * a.r - b.r * b.r) / (T(2) * d * a.r), T(-1), T(1)));
    T y = std::acos(std::clamp((d * d + b.r * b.r - a.r * a.r) / (T(2) * d * b.r), T(-1), T(1)));
    return a.r * a.r * x + b.r * b.r * y - d * a.r * std::sin(x);
}

template <class T>
struct Tangents {
    bool infinite = false;
    std::vector<Line<T>> lines{};
};

template <class T>
Tangents<T> tangents(Point<T> p, const Circle<T> &c) {
    static_assert(!std::is_integral_v<T>, "tangent coordinates need floating T");
    if (c.r == 0) {
        if (p == c.o) return {true, {}};
        return {false, {Line<T>(p, c.o)}};
    }
    auto v = p - c.o;
    T d2 = v.len2(), h2 = d2 - c.r * c.r;
    if (h2 == 0) return {false, {Line<T>::fromVec(p, v.rot90())}};
    if (h2 < 0) return {};
    auto base = c.o + v * (c.r * c.r / d2);
    auto w = v.rot90() * (c.r * std::sqrt(h2) / d2);
    return {false, {Line<T>(p, base - w), Line<T>(p, base + w)}};
}

template <class T>
Tangents<T> commonTangents(const Circle<T> &a, const Circle<T> &b) {
    static_assert(!std::is_integral_v<T>, "tangent coordinates need floating T");
    if (a.o == b.o) return {a.r == b.r, {}};
    if (a.r == 0) return tangents(a.o, b);
    if (b.r == 0) return tangents(b.o, a);
    Tangents<T> ans;
    ans.lines.reserve(4);
    auto d = b.o - a.o;
    T d2 = d.len2();
    for (int side : {1, -1}) {
        T dr = a.r - T(side) * b.r, h2 = d2 - dr * dr;
        if (h2 != 0 && h2 < 0) continue;
        T h = std::sqrt(std::max(T(0), h2));
        for (int sign : {1, -1}) {
            auto normal = (d * dr + d.rot90() * (h * T(sign))) / d2;
            ans.lines.push_back(Line<T>::fromVec(a.o + normal * a.r, normal.rot90()));
            if (h2 == 0) break;
        }
    }
    return ans;
}

template <class T>
T intersectionArea(const Circle<T> &c, const std::vector<Point<T>> &ps) {
    static_assert(!std::is_integral_v<T>, "intersection area needs floating T");
    T sum = 0;
    for (int i = 0, n = int(ps.size()); i < n; ++i) {
        auto a = ps[i] - c.o, b = ps[(i + 1) % n] - c.o, d = b - a;
        if (a.len2() <= c.r * c.r && b.len2() <= c.r * c.r) {
            sum += a.cross(b) / T(2);
            continue;
        }
        T d2 = d.len2();
        if (d2 == 0) continue;
        auto sector = [&](Vec<T> u, Vec<T> v) { return std::atan2(u.cross(v), u.dot(v)) * c.r * c.r / T(2); };
        T x = d.dot(a) / d2, det = x * x - (a.len2() - c.r * c.r) / d2;
        if (det <= 0) { sum += sector(a, b); continue; }
        T s = std::max(T(0), -x - std::sqrt(det)), t = std::min(T(1), -x + std::sqrt(det));
        if (t <= s) { sum += sector(a, b); continue; }
        auto u = a + d * s, v = a + d * t;
        sum += sector(a, u) + u.cross(v) / T(2) + sector(v, b);
    }
    return std::abs(sum);
}

template <class T>
T intersectionArea(const Circle<T> &c, const Polygon<T> &p) { return intersectionArea(c, p.ps); }

template <class T>
T intersectionArea(const Polygon<T> &p, const Circle<T> &c) { return intersectionArea(c, p); }

template <class T>
T intersectionArea(const Circle<T> &c, const Convex<T> &h) { return intersectionArea(c, h.vertices()); }

template <class T>
T intersectionArea(const Convex<T> &h, const Circle<T> &c) { return intersectionArea(c, h); }

template <class T>
Near<T> near(Point<T> p, const Circle<T> &c) {
    static_assert(!std::is_integral_v<T>, "nearest coordinates need floating T");
    if (c.loc(p) == Location::ON) return {p, p};
    auto v = p - c.o;
    auto q = v == Vec<T>::O ? c.o + Vec<T>{c.r, 0} : c.o + v * (c.r / v.len());
    return {p, q};
}

template <class T>
Near<T> near(const Circle<T> &c, const Line<T> &l) {
    auto hit = inter(c, l);
    if (hit) return {hit.ps[0], hit.ps[0]};
    auto p = near(c.o, l).second;
    return {near(p, c).second, p};
}

template <class T>
Near<T> near(const Circle<T> &c, const Seg<T> &s) {
    auto hit = inter(c, s);
    if (hit) return {hit.ps[0], hit.ps[0]};
    auto p = near(c.o, s).second;
    if (c.loc(s.a) == Location::IN && c.loc(s.b) == Location::IN)
        p = c.o.dist2(s.a) > c.o.dist2(s.b) ? s.a : s.b;
    return {near(p, c).second, p};
}

template <class T>
Near<T> near(const Circle<T> &a, const Circle<T> &b) {
    auto hit = inter(a, b);
    if (hit.kind == HitKind::CO) { auto p = a.o + Vec<T>{a.r, 0}; return {p, p}; }
    if (hit) return {hit.ps[0], hit.ps[0]};
    auto v = b.o - a.o;
    T d = v.len();
    v = d == 0 ? Vec<T>{1, 0} : v / d;
    if (d > a.r + b.r) return {a.o + v * a.r, b.o - v * b.r};
    if (a.r < b.r) v = -v;
    return {a.o + v * a.r, b.o + v * b.r};
}

template <class T>
bool inter(Point<T> p, const Circle<T> &s, std::nullptr_t) { return s.loc(p) == Location::ON; }

template <class T>
Hit<T> inter(Point<T> p, const Circle<T> &s) { return inter(p, s, nullptr) ? Hit<T>{HitKind::ONE, {p, {}}} : Hit<T>{}; }

// The circumference intersects the filled region iff it spans its radial range.
template <class T>
bool inter(const Circle<T> &c, const Polygon<T> &p, std::nullptr_t) {
    bool reaches = false;
    const auto &ps = p.ps;
    for (auto q : ps) reaches |= c.o.dist2(q) >= c.r * c.r;
    if (!reaches) return false;
    if (p.loc(c.o) != Location::OUT) return true;
    for (int i = 0; i < p.size(); ++i)
        if (inter(c, Seg<T>{ps[i], ps[(i + 1) % p.size()]}, nullptr)) return true;
    return false;
}

template <class T>
Near<T> near(const Circle<T> &c, const Polygon<T> &p) {
    assert(p.size());
    auto onCircle = c.o + Vec<T>{c.r, 0};
    if (p.loc(onCircle) != Location::OUT) return {onCircle, onCircle};
    const auto &ps = p.ps;
    auto best = near(c, ps[0]);
    for (int i = 0; i < p.size(); ++i) {
        auto q = near(c, Seg<T>{ps[i], ps[(i + 1) % p.size()]});
        if (q.first.dist2(q.second) < best.first.dist2(best.second)) best = q;
        if (best.first == best.second) break;
    }
    return best;
}

// The circumference intersects the filled region iff it spans its radial range.
template <class T>
bool inter(const Circle<T> &c, const Convex<T> &p, std::nullptr_t) {
    bool reaches = false;
    const auto &ps = p.vertices();
    for (auto q : ps) reaches |= c.o.dist2(q) >= c.r * c.r;
    if (!reaches) return false;
    if (p.loc(c.o) != Location::OUT) return true;
    for (int i = 0; i < p.size(); ++i)
        if (inter(c, Seg<T>{ps[i], ps[(i + 1) % p.size()]}, nullptr)) return true;
    return false;
}

template <class T>
Near<T> near(const Circle<T> &c, const Convex<T> &p) {
    assert(p.size());
    auto onCircle = c.o + Vec<T>{c.r, 0};
    if (p.loc(onCircle) != Location::OUT) return {onCircle, onCircle};
    const auto &ps = p.vertices();
    auto best = near(c, ps[0]);
    for (int i = 0; i < p.size(); ++i) {
        auto q = near(c, Seg<T>{ps[i], ps[(i + 1) % p.size()]});
        if (q.first.dist2(q.second) < best.first.dist2(best.second)) best = q;
        if (best.first == best.second) break;
    }
    return best;
}

GEO2_REVERSE(Circle, Point)
GEO2_REVERSE_INTER(Circle, Point)

GEO2_REVERSE(Line, Circle)
GEO2_REVERSE_INTER(Line, Circle)
GEO2_REVERSE(Seg, Circle)
GEO2_REVERSE_INTER(Seg, Circle)
GEO2_REVERSE(Convex, Circle)
GEO2_REVERSE(Polygon, Circle)

#undef GEO2_REVERSE_INTER
#undef GEO2_REVERSE

} // namespace _geo2

using _geo2::Circle;
using _geo2::Tangents;
using _geo2::CircleRelation;
using _geo2::inter;
using _geo2::near;
using _geo2::tangents;
using _geo2::commonTangents;
using _geo2::relation;
using _geo2::intersectionArea;
// SNIPPET END
