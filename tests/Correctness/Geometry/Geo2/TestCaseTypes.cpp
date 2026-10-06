#include "../../../../src/Geometry/Geo2/Circle.hpp"
#include "../../../Support/TestSupport.hpp"
#include "../../../Support/CaseSupport.hpp"
#include <tuple>
using i64 = long long;

template <class T> long double scalar(T x) {
    if constexpr (std::is_arithmetic_v<T>) return x;
    else return x.val();
}
template <class T> bool belongs(Point<T> p, Point<T> q) { return p == q; }
template <class T> bool belongs(const Line<T> &l, Point<T> p) { return std::abs(scalar(l.eval(p))) < 1E-7; }
template <class T> bool belongs(const Seg<T> &s, Point<T> p) {
    return std::abs(scalar(s.a.cross(s.b, p))) < 1E-7 && scalar((p - s.a).dot(p - s.b)) < 1E-7;
}
template <class T> bool belongs(const Circle<T> &c, Point<T> p) { return std::abs(scalar(c.o.dist2(p) - c.r * c.r)) < 1E-7; }
template <class T> bool belongs(const Polygon<T> &p, Point<T> q) { return p.loc(q) != Location::OUT; }
template <class T> bool belongs(const Convex<T> &h, Point<T> q) { return h.loc(q) != Location::OUT; }

template <class A, class B>
void checkNear(const A &a, const B &b) {
    auto [p, q] = ::near(a, b);
    auto [v, u] = ::near(b, a);
    CHECK(belongs(a, p) && belongs(b, q) && belongs(a, u) && belongs(b, v));
    CHECK(std::abs(scalar(p.dist(q)) - scalar(u.dist(v))) < 1E-7);
    CHECK(::inter(a, b, nullptr) == (scalar(p.dist2(q)) < 1E-14));
}

template <class T, class = void> struct HasSupport : std::false_type {};
template <class T> struct HasSupport<T, std::void_t<decltype(std::declval<T>().support(Vec<double>{}))>> : std::true_type {};
template <class T, class = void> struct HasEdge : std::false_type {};
template <class T> struct HasEdge<T, std::void_t<decltype(std::declval<T>().edge(0))>> : std::true_type {};
template <class T, class = void> struct HasDist : std::false_type {};
template <class T> struct HasDist<T, std::void_t<decltype(std::declval<T>().dist(Point<double>{}))>> : std::true_type {};

template <class T>
void instantiate() {
    Point<T> q{5, 3};
    Line<T> a({0, 0}, {2, 0}), b({1, -1}, {1, 1});
    Seg<T> s{{0, 0}, {2, 0}}, t{{1, -1}, {1, 1}};
    auto h = ::convexHull<T>({{0, 0}, {2, 0}, {2, 2}, {0, 2}});
    Polygon<T> p{{{3, 0}, {6, 0}, {6, 4}, {3, 4}}};
    Circle<T> c({0, 0}, 1), d({2, 0}, 1);
    auto objects = std::make_tuple(q, a, s, h, p, c);
    auto row = [&](const auto &x) { std::apply([&](const auto &...y) { (checkNear(x, y), ...); }, objects); };
    std::apply([&](const auto &...x) { (row(x), ...); }, objects);
    CHECK(::minBoundingRect(h)->area == T(4) && ::minWidth(h) == T(2));
    CHECK(::farthestPair(h).has_value());
    CHECK(::halfPlaneIntersection(std::vector<Line<T>>{a}, h).area() == T(4));
    CHECK(::minkowskiSum(h, h).area() == T(16));
    CHECK(a.loc(s.mid()) == Location::ON && a.parallel(a.reversed()) && a.orthogonal(b));
    CHECK(a.reflect({1, 1}) == Point<T>(1, -1));
    CHECK(::near(Point<T>{1, 3}, s).second == Point<T>(1, 0));
    CHECK(::inter(s, t).kind == HitKind::ONE);
    CHECK(p.isConvex() && p.centroid() == Point<T>(T(4.5), 2));
    CHECK(::near(Point<T>{3, 1}, h).second == Point<T>(2, 1));
    CHECK(::inter(h, t).kind == HitKind::SEG && ::inter(t, h).kind == HitKind::SEG);
    CHECK(::inter(p, t).empty() && ::inter(t, p).empty());
    CHECK(::inter(a, c).size() == 2 && ::inter(s, c).size() == 1);
    CHECK(::inter(c, d).kind == HitKind::ONE && ::inter(c, d, nullptr));
    CHECK(Circle<T>::circum({0, 0}, {2, 0}, {0, 2}).o == Point<T>(1, 1));
    CHECK(::near(Point<T>{0, 0}, c).second == Point<T>(1, 0));
    CHECK(c.loc(c.o) == Location::IN && !::inter(c.o, c, nullptr));
    CHECK(::near(c, Circle<T>({0, 0}, 2)).first.dist(::near(c, Circle<T>({0, 0}, 2)).second) == T(1));
    CHECK(!::inter(c, Seg<T>{{0, 0}, {T(.5), 0}}, nullptr));
    CHECK(!::inter(c, Circle<T>({0, 0}, 2), nullptr));
    CHECK(!::inter(c, Convex<T>{}, nullptr));
    CHECK(::inter(c, h, nullptr) && ::inter(h, c, nullptr));
    CHECK(std::abs(::intersectionArea(c, h) - ::intersectionArea(h, c)) < T(1E-5));
    static_assert(std::is_same_v<decltype(::near(q, s)), Near<T>>);
    static_assert(std::is_same_v<decltype(::inter(s, t)), Hit<T>>);
    static_assert(std::is_same_v<decltype(::inter(s, t, nullptr)), bool>);
}

int isolatedCase() {
    static_assert(!HasSupport<Convex<double>>::value);
    static_assert(!HasEdge<Convex<double>>::value && !HasEdge<Polygon<double>>::value);
    static_assert(!HasDist<Line<double>>::value && !HasDist<Seg<double>>::value);
    static_assert(!HasDist<Circle<double>>::value && !HasDist<Convex<double>>::value);
    instantiate<double>();
    instantiate<long double>();
    instantiate<FloatPointNumber<double>>();
    instantiate<FloatPointNumber<long double>>();
    auto h = ::convexHull<i64>({{0, 0}, {4, 0}, {4, 4}, {0, 4}});
    auto objects = std::make_tuple(Point<i64>{1, 1}, Line<i64>({0, 0}, {1, 1}), Seg<i64>{{0, 0}, {1, 0}}, h, h.polygon(), Circle<i64>({0, 0}, 2));
    auto row = [&](const auto &x) { std::apply([&](const auto &...y) { (CHECK(::inter(x, y, nullptr) == ::inter(y, x, nullptr)), ...); }, objects); };
    std::apply([&](const auto &...x) { (row(x), ...); }, objects);
    std::cout << "Geo2: native/wrapped double/long-double near matrix, integer inter matrix, private API PASS\n";
    return 0;
}
int main() {
    runCase("Geo2/types", [] { CHECK(isolatedCase() == 0); });
    return 0;
}
