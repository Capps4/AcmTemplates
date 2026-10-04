#pragma once
#include <cassert>
#include <cmath>
#include <type_traits>
#include "../FloatPointNumber/Final.hpp"

// SNIPPET BEGIN
template <class T, class G, class F>
struct PointImpl {
    using Value = T;
    using Wide = G;
    using Real = F;
    using Wide3 = std::conditional_t<std::is_integral_v<T>, __int128, G>;

    T x, y;

    constexpr PointImpl() : x(), y() {}

    constexpr PointImpl(T x, T y) : x(x), y(y) {}

    constexpr PointImpl operator+() const { return *this; }

    constexpr PointImpl operator-() const { return PointImpl(-x, -y); }

    constexpr PointImpl &operator+=(PointImpl p) {
        x += p.x;
        y += p.y;
        return *this;
    }

    constexpr PointImpl &operator-=(PointImpl p) {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    constexpr PointImpl &operator*=(T k) {
        x *= k;
        y *= k;
        return *this;
    }

    friend constexpr PointImpl operator+(PointImpl a, PointImpl b) { return a += b; }

    friend constexpr PointImpl operator-(PointImpl a, PointImpl b) { return a -= b; }

    friend constexpr PointImpl operator*(PointImpl p, T k) { return p *= k; }

    friend constexpr PointImpl operator*(T k, PointImpl p) { return p *= k; }

    // 返回从当前点指向目标点的向量。
    constexpr PointImpl to(PointImpl p) const { return p - *this; }

    constexpr Wide dot(PointImpl p) const { return Wide(x) * p.x + Wide(y) * p.y; }

    constexpr Wide cross(PointImpl p) const { return Wide(x) * p.y - Wide(y) * p.x; }

    constexpr Wide cross(PointImpl a, PointImpl b) const {
        Wide ax = Wide(a.x) - Wide(x), ay = Wide(a.y) - Wide(y);
        Wide bx = Wide(b.x) - Wide(x), by = Wide(b.y) - Wide(y);
        return ax * by - ay * bx;
    }

    constexpr Wide len2() const { return dot(*this); }

    Real len() const {
        Real v(len2());
        return std::sqrt(v);
    }

    constexpr Wide dist2(PointImpl p) const {
        Wide dx = Wide(p.x) - Wide(x), dy = Wide(p.y) - Wide(y);
        return dx * dx + dy * dy;
    }

    Real dist(PointImpl p) const {
        Real v(dist2(p));
        return std::sqrt(v);
    }

    constexpr PointImpl rot90() const { return PointImpl(-y, x); }

    PointImpl rot(Real ang) const {
        static_assert(!std::is_integral_v<T>, "Point::rot needs floating Point");
        Real c = std::cos(ang), s = std::sin(ang);
        return PointImpl(T(x * c - y * s), T(x * s + y * c));
    }

    // 返回两个非零向量的有向夹角。
    Real angle(PointImpl p) const {
        assert(*this != O and p != O);
        Real cr(cross(p)), dt(dot(p));
        return std::atan2(cr, dt);
    }

    // 返回向量所在的极角半平面。
    constexpr int half() const {
        if (y < 0) return 1;
        if (y > 0) return 0;
        return x < 0;
    }

    constexpr bool operator==(PointImpl p) const { return x == p.x and y == p.y; }

    constexpr bool operator!=(PointImpl p) const { return !(*this == p); }

    constexpr bool operator<(PointImpl p) const {
        if (x < p.x) return true;
        if (p.x < x) return false;
        return y < p.y;
    }

    template <class Input>
    friend Input &operator>>(Input &in, PointImpl &p) {
        in >> p.x >> p.y;
        return in;
    }

    template <class Output>
    friend Output &operator<<(Output &out, PointImpl p) {
        out << '(' << p.x << ", " << p.y << ')';
        return out;
    }

    static const PointImpl O;
};

template <class T, class G, class F>
inline const PointImpl<T, G, F> PointImpl<T, G, F>::O = PointImpl<T, G, F>();

using Point = PointImpl<int, long long, Float>;
using Vec = Point;
