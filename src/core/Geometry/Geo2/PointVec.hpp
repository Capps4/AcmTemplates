#pragma once
#include "Include.hpp"

// SNIPPET BEGIN
namespace _geo2 {

template <class T, bool IsPoint>
struct Coord;

template <class T>
using Point = Coord<T, true>;

template <class T>
using Vec = Coord<T, false>;

template <class T, bool IsPoint>
struct Coord {
    using Scalar = T;
    using P = Point<T>;
    using V = Vec<T>;

    T x{}, y{};

    constexpr Coord() = default;
    constexpr Coord(T x, T y) : x(x), y(y) {}

    constexpr Coord &operator+=(V v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr Coord &operator-=(V v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    constexpr Coord &operator*=(T k) {
        static_assert(!IsPoint, "Only vectors can be scaled");
        x *= k;
        y *= k;
        return *this;
    }

    constexpr Coord &operator/=(T k) {
        static_assert(!IsPoint, "Only vectors can be scaled");
        x /= k;
        y /= k;
        return *this;
    }

    // Return the position vector relative to the origin.
    constexpr V toVec() const {
        static_assert(IsPoint, "toVec needs a point");
        return V(x, y);
    }

    // Return the displacement from this point to p.
    constexpr V to(P p) const {
        static_assert(IsPoint, "to needs a point");
        return V(p.x - x, p.y - y);
    }

    constexpr T dot(V v) const {
        static_assert(!IsPoint, "dot needs a vector");
        if constexpr (std::is_integral_v<T>)
            return T(__int128_t(x) * v.x + __int128_t(y) * v.y);
        else
            return x * v.x + y * v.y;
    }

    constexpr T cross(V v) const {
        static_assert(!IsPoint, "cross(v) needs a vector");
        if constexpr (std::is_integral_v<T>) {
            return T(__int128_t(x) * v.y - __int128_t(y) * v.x);
        } else {
            // Keep C++17 constant evaluation; fma is a runtime operation.
            if (__builtin_is_constant_evaluated())
                return x * v.y - y * v.x;
            // Compensate the rounded product before subtracting close products.
            T p = y * v.x;
            return std::fma(x, v.y, -p) + std::fma(-y, v.x, p);
        }
    }

    // Return the signed cross product of the displacements to a and b.
    constexpr T cross(P a, P b) const {
        static_assert(IsPoint, "cross(a, b) needs a point");
        return to(a).cross(to(b));
    }

    constexpr T len2() const {
        return dot(*this);
    }
    auto len() const {
        return std::sqrt(len2());
    }

    constexpr T dist2(P p) const {
        return to(p).len2();
    }
    auto dist(P p) const {
        return std::sqrt(dist2(p));
    }

    constexpr V rot90() const {
        static_assert(!IsPoint, "rot90 needs a vector");
        return V(-y, x);
    }

    V rot(T ang) const {
        static_assert(!IsPoint, "rot needs a vector");
        static_assert(!std::is_integral_v<T>, "rot needs floating coordinates");
        auto c = std::cos(ang), s = std::sin(ang);
        return V(x * c - y * s, x * s + y * c);
    }

    // Return the signed angle between two nonzero vectors.
    auto angle(V v) const {
        static_assert(!IsPoint, "angle needs a vector");
        assert(*this != O and v != V::O);
        return std::atan2(cross(v), dot(v));
    }

    // Return the half-plane used for polar angle ordering.
    constexpr int half() const {
        static_assert(!IsPoint, "half needs a vector");
        if (y < 0)
            return 1;
        if (y > 0)
            return 0;
        return x < 0;
    }

    constexpr bool operator==(Coord p) const {
        return x == p.x and y == p.y;
    }

    constexpr bool operator!=(Coord p) const {
        return !(*this == p);
    }

    constexpr bool operator<(Coord p) const {
        if (x < p.x)
            return true;
        if (p.x < x)
            return false;
        return y < p.y;
    }

    template <class Input>
    friend Input &operator>>(Input &in, Coord &p) {
        in >> p.x >> p.y;
        return in;
    }

    template <class Output>
    friend Output &operator<<(Output &out, Coord p) {
        out << '(' << p.x << ", " << p.y << ')';
        return out;
    }

    static const Coord O;
};

template <class T, bool IsPoint>
const Coord<T, IsPoint> Coord<T, IsPoint>::O{};

// Operators live beside Coord so argument-dependent lookup can find them.
template <class T>
constexpr Vec<T> operator-(Point<T> a, Point<T> b) {
    return Vec<T>(a.x - b.x, a.y - b.y);
}

template <class T>
constexpr Point<T> operator+(Point<T> p, Vec<T> v) {
    return p += v;
}

template <class T>
constexpr Point<T> operator+(Vec<T> v, Point<T> p) {
    return p += v;
}

template <class T>
constexpr Point<T> operator-(Point<T> p, Vec<T> v) {
    return p -= v;
}

template <class T>
constexpr Vec<T> operator+(Vec<T> a, Vec<T> b) {
    return a += b;
}

template <class T>
constexpr Vec<T> operator-(Vec<T> a, Vec<T> b) {
    return a -= b;
}

template <class T>
constexpr Vec<T> operator+(Vec<T> v) {
    return v;
}

template <class T>
constexpr Vec<T> operator-(Vec<T> v) {
    return Vec<T>(-v.x, -v.y);
}

template <class T>
constexpr Vec<T> operator*(Vec<T> v, typename Vec<T>::Scalar k) {
    return v *= k;
}

template <class T>
constexpr Vec<T> operator*(typename Vec<T>::Scalar k, Vec<T> v) {
    return v *= k;
}

template <class T>
constexpr Vec<T> operator/(Vec<T> v, typename Vec<T>::Scalar k) {
    return v /= k;
}

enum class Location { OUT, ON, IN };
enum class HitKind { NONE, ONE, TWO, SEG, CO };

template <class T>
struct Hit {
    HitKind kind = HitKind::NONE;
    std::array<Point<T>, 2> ps{};
    int size() const {
        return kind == HitKind::ONE ? 1 : kind == HitKind::TWO or kind == HitKind::SEG ? 2 : 0;
    }
    explicit operator bool() const {
        return kind != HitKind::NONE;
    }
};

template <class T>
int orient(Point<T> a, Point<T> b, Point<T> c) {
    if constexpr (std::is_integral_v<T>) {
        auto x = (__int128_t(b.x) - a.x) * (__int128_t(c.y) - a.y) -
                 (__int128_t(b.y) - a.y) * (__int128_t(c.x) - a.x);
        return (x > 0) - (x < 0);
    } else {
        auto x = a.cross(b, c);
        return x == 0 ? 0 : x < 0 ? -1 : 1;
    }
}

template <class T>
using Near = std::pair<Point<T>, Point<T>>;

template <class T>
Near<T> near(Point<T> p, Point<T> q) {
    return {p, q};
}

template <class T>
bool inter(Point<T> p, Point<T> q, std::nullptr_t) {
    return p == q;
}

template <class T>
Hit<T> inter(Point<T> p, Point<T> q) {
    return p == q ? Hit<T>{HitKind::ONE, {p, {}}} : Hit<T>{};
}

// Reverse overloads only forward; witness order follows the arguments.
#define GEO2_REVERSE(A, B) \
    template <class T> \
    bool inter(const A<T> &a, const B<T> &b, std::nullptr_t) { \
        return inter(b, a, nullptr); \
    } \
    template <class T> \
    Near<T> near(const A<T> &a, const B<T> &b) { \
        auto q = near(b, a); \
        return {q.second, q.first}; \
    }
#define GEO2_REVERSE_INTER(A, B) \
    template <class T> \
    auto inter(const A<T> &a, const B<T> &b) -> decltype(inter(b, a)) { \
        return inter(b, a); \
    }

} // namespace _geo2

using _geo2::Point;
using _geo2::Vec;
using _geo2::Location;
using _geo2::HitKind;
using _geo2::Hit;
using _geo2::Near;
using _geo2::orient;
using _geo2::inter;
using _geo2::near;
// SNIPPET END
