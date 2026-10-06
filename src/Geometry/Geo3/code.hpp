#pragma once
#include "Include.hpp"

// SNIPPET BEGIN
namespace _geo3 {
template <class T, bool IsPoint>
struct Coord;
}

template <class T>
using Point = _geo3::Coord<T, true>;
template <class T>
using Vec = _geo3::Coord<T, false>;

namespace _geo3 {
template <class T, bool IsPoint>
struct Coord {
    using Value = T;
    using Wide = std::conditional_t<std::is_integral_v<T>,
                                   std::conditional_t<(sizeof(T) <= 4), long long, __int128_t>, T>;
    using Wide3 = std::conditional_t<std::is_integral_v<T>, __int128_t, T>;
    using Real = std::conditional_t<std::is_integral_v<T>, Float, T>;
    using CrossType = Vec<Wide>;
    using P = Point<T>;
    using V = Vec<T>;

    T x{}, y{}, z{};
    constexpr Coord() = default;
    constexpr Coord(T x, T y, T z) : x(x), y(y), z(z) {}

    Coord &operator+=(V v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Coord &operator-=(V v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    Coord &operator*=(T k) {
        static_assert(!IsPoint, "Only vectors can be scaled");
        x *= k;
        y *= k;
        z *= k;
        return *this;
    }
    Coord &operator/=(T k) {
        static_assert(!IsPoint, "Only vectors can be scaled");
        x /= k;
        y /= k;
        z /= k;
        return *this;
    }
    V toVec() const {
        static_assert(IsPoint, "toVec needs a point");
        return {x, y, z};
    }
    V to(P p) const {
        static_assert(IsPoint, "to needs a point");
        return {p.x - x, p.y - y, p.z - z};
    }
    Wide dot(V v) const {
        static_assert(!IsPoint, "dot needs a vector");
        return Wide(x) * v.x + Wide(y) * v.y + Wide(z) * v.z;
    }
    CrossType cross(V v) const {
        static_assert(!IsPoint, "cross needs a vector");
        return {Wide(y) * v.z - Wide(z) * v.y, Wide(z) * v.x - Wide(x) * v.z,
                Wide(x) * v.y - Wide(y) * v.x};
    }
    Wide3 triple(V b, V c) const {
        static_assert(!IsPoint, "triple needs vectors");
        return Wide3(x) * (Wide3(b.y) * c.z - Wide3(b.z) * c.y) +
               Wide3(y) * (Wide3(b.z) * c.x - Wide3(b.x) * c.z) +
               Wide3(z) * (Wide3(b.x) * c.y - Wide3(b.y) * c.x);
    }
    Wide len2() const {
        static_assert(!IsPoint, "len2 needs a vector");
        return dot(*this);
    }
    Real len() const {
        return std::sqrt(Real(len2()));
    }
    Wide dist2(P p) const {
        return to(p).len2();
    }
    Real dist(P p) const {
        return std::sqrt(Real(dist2(p)));
    }
    bool operator==(Coord p) const {
        return x == p.x and y == p.y and z == p.z;
    }
    bool operator!=(Coord p) const {
        return !(*this == p);
    }
    bool operator<(Coord p) const {
        if (x != p.x)
            return x < p.x;
        if (y != p.y)
            return y < p.y;
        return z < p.z;
    }
    template <class Input>
    friend Input &operator>>(Input &in, Coord &p) {
        in >> p.x >> p.y >> p.z;
        return in;
    }
    template <class Output>
    friend Output &operator<<(Output &out, Coord p) {
        out << '(' << p.x << ", " << p.y << ", " << p.z << ')';
        return out;
    }
    static const Coord O;
};

template <class T, bool IsPoint>
const Coord<T, IsPoint> Coord<T, IsPoint>::O{};

template <class T, bool IsPoint>
Coord<T, IsPoint> operator+(Coord<T, IsPoint> a, Vec<T> b) {
    return a += b;
}
template <class T>
Point<T> operator+(Vec<T> a, Point<T> b) {
    return b += a;
}
template <class T, bool IsPoint>
Coord<T, IsPoint> operator-(Coord<T, IsPoint> a, Vec<T> b) {
    return a -= b;
}
template <class T>
Vec<T> operator-(Point<T> a, Point<T> b) {
    return b.to(a);
}
template <class T>
Vec<T> operator-(Vec<T> v) {
    return {-v.x, -v.y, -v.z};
}
template <class T>
Vec<T> operator*(Vec<T> v, T k) {
    return v *= k;
}
template <class T>
Vec<T> operator*(T k, Vec<T> v) {
    return v *= k;
}
template <class T>
Vec<T> operator/(Vec<T> v, T k) {
    return v /= k;
}
}

template <class T>
struct Hit {
    using P = Point<T>;
    using PointType = Point<typename P::Real>;

    std::string type;
    std::vector<PointType> ps;

    explicit Hit(const std::string &type = "NO") : type(type), ps() {}
    Hit(const std::string &type, PointType p) : type(type), ps(1, p) {}
};

template <class T>
struct Line {
    using P = Point<T>;
    using PointType = P;
    using Value = typename P::Value;
    using Real = typename P::Real;

    P p;
    Vec<T> v;

    Line(P a, P b) : p(a), v(a.to(b)) {
        assert(a != b);
    }
    static Line fromVec(P p, Vec<T> v) {
        return Line(p, p + v);
    }
    Vec<T> vec() const {
        return v;
    }

    // 判断点在三维直线上或直线外。
    std::string loc(P q) const {
        typename P::CrossType c = vec().cross(p.to(q));
        return c == typename P::CrossType() ? "ON" : "OUT";
    }

    Real dist(P q) const {
        return vec().cross(p.to(q)).len() / vec().len();
    }

    // 返回点在三维直线上的垂足。
    P foot(P q) const {
        static_assert(!std::is_integral_v<typename P::Value>, "Line::foot needs floating Point");
        Vec<T> v = vec();
        Real k(v.dot(p.to(q)));
        k /= v.len2();
        return p + v * Value(k);
    }
};

template <class T>
struct Seg {
    using P = Point<T>;
    using Wide = typename P::Wide;
    using Real = typename P::Real;

    P a, b;

    Seg() : a(), b() {}
    Seg(P a, P b) : a(a), b(b) {}

    Vec<T> vec() const {
        return a.to(b);
    }

    Line<T> line() const {
        assert(a != b);
        return Line<T>(a, b);
    }

    // 判断点在三维线段上或线段外。
    std::string loc(P p) const {
        if (a == b)
            return a == p ? "ON" : "OUT";
        if (line().loc(p) == "OUT")
            return "OUT";
        Wide x = a.to(p).dot(b.to(p));
        return x <= 0 ? "ON" : "OUT";
    }

    Real dist(P p) const {
        if (a == b)
            return a.dist(p);
        Vec<T> v = vec();
        Wide x = v.dot(a.to(p));
        if (x <= 0)
            return a.dist(p);
        x = v.dot(b.to(p));
        if (x >= 0)
            return b.dist(p);
        return line().dist(p);
    }
};

template <class T>
struct Plane {
    using P = Point<T>;
    using Value = typename P::Value;
    using Wide = typename P::Wide;
    using Wide3 = typename P::Wide3;
    using Real = typename P::Real;
    using Normal = typename P::CrossType;

    P p;
    Normal n;

    Plane(P a, P b, P c) : p(a), n(a.to(b).cross(a.to(c))) {
        assert(n != Normal::O);
    }

    // 返回点代入平面方程后的有符号值。
    Wide3 eval(P q) const {
        return Wide3(n.x) * (q.x - p.x) + Wide3(n.y) * (q.y - p.y) + Wide3(n.z) * (q.z - p.z);
    }

    // 返回点在平面正侧、平面上或负侧的符号。
    int side(P q) const {
        Wide3 x = eval(q);
        if (x == 0)
            return 0;
        return x < 0 ? -1 : 1;
    }

    Real dist(P q) const {
        Real ans(eval(q));
        if (ans < 0)
            ans = -ans;
        return ans / n.len();
    }

    P foot(P q) const {
        static_assert(!std::is_integral_v<typename P::Value>, "Plane::foot needs floating Point");
        Real k(eval(q));
        k /= n.len2();
        Vec<T> v(Value(n.x), Value(n.y), Value(n.z));
        return q - v * Value(k);
    }

    // 返回平面法向量与给定向量的点积。
    Wide3 dot(Vec<T> v) const {
        return Wide3(n.x) * v.x + Wide3(n.y) * v.y + Wide3(n.z) * v.z;
    }

    // 返回平面与直线的关系及交点。
    Hit<T> relation(Line<T> l) const {
        Wide3 den = dot(l.vec());
        if (den == 0)
            return Hit<T>(side(l.p) == 0 ? "SAME" : "NO");

        using Q = typename Hit<T>::PointType;
        Q a(l.p.x, l.p.y, l.p.z);
        Vec<Real> v(l.v.x, l.v.y, l.v.z);
        Real k(-eval(l.p));
        k /= den;
        return Hit<T>("ONE", a + v * k);
    }
};

struct Face {
    int a, b, c;

    Face() : a(), b(), c() {}
    Face(int a, int b, int c) : a(a), b(b), c(c) {}
};

template <class T>
struct Polyhedron {
    using P = Point<T>;
    using Wide3 = typename P::Wide3;
    using Real = typename P::Real;

    std::vector<P> ps{};
    std::vector<Face> fs{};

    Real area() const {
        Real ans(0);
        for (auto i = 0U; i < fs.size(); i++) {
            const Face &f = fs[i];
            assert(f.a >= 0 and f.b >= 0 and f.c >= 0);
            std::size_t a = static_cast<std::size_t>(f.a);
            std::size_t b = static_cast<std::size_t>(f.b);
            std::size_t c = static_cast<std::size_t>(f.c);
            assert(a < ps.size() and b < ps.size() and c < ps.size());
            ans += ps[a].to(ps[b]).cross(ps[a].to(ps[c])).len() / 2;
        }
        return ans;
    }

    // 返回六倍有符号体积。
    Wide3 vol6() const {
        Wide3 ans = 0;
        for (auto i = 0U; i < fs.size(); i++) {
            const Face &f = fs[i];
            assert(f.a >= 0 and f.b >= 0 and f.c >= 0);
            std::size_t a = static_cast<std::size_t>(f.a);
            std::size_t b = static_cast<std::size_t>(f.b);
            std::size_t c = static_cast<std::size_t>(f.c);
            assert(a < ps.size() and b < ps.size() and c < ps.size());
            ans += ps[a].toVec().triple(ps[b].toVec(), ps[c].toVec());
        }
        return ans;
    }

    Real vol() const {
        Real ans(vol6());
        if (ans < 0)
            ans = -ans;
        return ans / 6;
    }

    // 反转所有三角面的方向。
    void reverse() {
        for (auto i = 0U; i < fs.size(); i++)
            std::swap(fs[i].b, fs[i].c);
    }
};

