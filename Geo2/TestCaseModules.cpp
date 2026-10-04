#ifndef GEO2_TEST_LAYER
#define GEO2_TEST_LAYER 4
#endif

#if GEO2_TEST_LAYER == 1
#include "PointVec.hpp"
#elif GEO2_TEST_LAYER == 2
#include "SegLine.hpp"
#elif GEO2_TEST_LAYER == 3
#include "PolygonConvex.hpp"
#else
#include "Circle.hpp"
#endif
#include "../TestSupport.hpp"
#include <sstream>

using i64 = long long;

template <class T>
void checkLayer() {
    Point<T> p{0, 0}, q{2, 0};
    Vec<T> v{2, 0};
    static_assert(std::is_same_v<decltype(q - p), Vec<T>>);
    CHECK(p + v == q && v + p == q);
    CHECK(::orient(p, q, Point<T>{1, 1}) == 1);
    CHECK(::inter(p, p).kind == HitKind::ONE && !::inter(p, q, nullptr));
    CHECK(::near(p, q) == Near<T>(p, q));
    Hit<T> empty;
    CHECK(!empty && empty.size() == 0);
    std::istringstream input("3 4");
    input >> p;
    CHECK(p == Point<T>(3, 4));

#if GEO2_TEST_LAYER >= 2
    Line<T> l({0, 0}, {2, 0});
    Seg<T> s{{0, -1}, {0, 1}};
    CHECK(::inter(l, s, nullptr) && ::inter(s, l, nullptr));
    CHECK(::inter(l, Point<T>{0, 0}).kind == HitKind::ONE);
    if constexpr (!std::is_integral_v<T>) {
        CHECK(::inter(l, s).ps[0] == Point<T>::O);
        CHECK(::near(s, l).first == Point<T>::O);
        CHECK(::near(l, s).second == Point<T>::O);
        CHECK(::near(p, l).second == Point<T>(3, 0));
    }
#endif

#if GEO2_TEST_LAYER >= 3
    Polygon<T> polygon{{{0, 0}, {2, 0}, {2, 2}, {0, 2}}};
    auto hull = ::convexHull<T>({{2, 2}, {0, 0}, {2, 0}, {0, 2}, {1, 1}, {0, 0}});
    static_assert(std::is_same_v<decltype(polygon[0]), Point<T> &>);
    static_assert(std::is_same_v<decltype(std::as_const(polygon)[0]), const Point<T> &>);
    static_assert(std::is_same_v<decltype(*polygon.begin()), Point<T> &>);
    static_assert(std::is_same_v<decltype(*std::as_const(polygon).begin()), const Point<T> &>);
    static_assert(std::is_same_v<decltype(hull[0]), const Point<T> &>);
    static_assert(std::is_same_v<decltype(*hull.begin()), const Point<T> &>);
    static_assert(std::is_same_v<decltype(*std::as_const(hull).begin()), const Point<T> &>);
    static_assert(!std::is_assignable_v<decltype(hull[0]), Point<T>>);
    static_assert(!std::is_assignable_v<decltype(*hull.begin()), Point<T>>);
    CHECK(std::equal(polygon.begin(), polygon.end(), hull.begin(), hull.end()));
    CHECK(std::distance(hull.cbegin(), hull.cend()) == hull.size());
    CHECK(hull.end() - hull.begin() == hull.size());
    CHECK(hull.begin()[2] == hull[2]);
    CHECK(polygon.front() == hull.front() && polygon.back() == hull.back());
    std::vector<Point<T>> copied(hull.begin(), hull.end());
    CHECK(copied == hull.vertices());
    auto duplicate = hull;
    auto moved = std::move(duplicate);
    CHECK(std::equal(moved.begin(), moved.end(), copied.begin(), copied.end()));
    CHECK(moved.loc({1, 1}) == Location::IN);
    for (auto &vertex : polygon) vertex += Vec<T>{3, 0};
    CHECK(polygon.front() == Point<T>(3, 0) && polygon.area2() == T(8));
    polygon[0] -= Vec<T>{3, 0};
    CHECK(polygon[0] == Point<T>::O);
    std::reverse(polygon.begin(), polygon.end());
    CHECK(polygon.front() == Point<T>(3, 2));
    const auto &constPolygon = polygon;
    CHECK(std::equal(constPolygon.cbegin(), constPolygon.cend(), polygon.begin(), polygon.end()));
    Polygon<T> emptyPolygon;
    CHECK(emptyPolygon.empty() && emptyPolygon.begin() == emptyPolygon.end());
    Convex<T> none;
    CHECK(none.empty() && none.begin() == none.end());
    auto singleton = ::convexHull<T>({{1, 2}});
    auto segment = Convex<T>::fromBoundary({{2, 0}, {1, 0}, {0, 0}});
    CHECK(singleton.front() == singleton.back() && singleton.size() == 1);
    CHECK(segment.size() == 2 && segment.front() == Point<T>::O && segment.back() == q);
    CHECK(::farthestPair(hull).has_value() && ::diameter(hull) > 0);
    CHECK(::minWidth(hull) == T(2));
    CHECK(::minkowskiSum(hull, hull).area2() == T(32));
    CHECK(::minkowskiDiff(hull, hull).area2() == T(32));
    CHECK(::inter(hull, l, nullptr) && ::inter(l, hull, nullptr));
    if constexpr (!std::is_integral_v<T>) {
        CHECK(::minBoundingRect(hull)->area == T(4));
        CHECK(::cutLeft(hull, l).area() == T(4));
        CHECK(::halfPlaneIntersection(std::vector<Line<T>>{l}, hull).area() == T(4));
        auto negativeHull = ::convexHull<T>({{-4, -4}, {-2, -4}, {-2, -2}, {-4, -2}});
        auto clipped = ::halfPlaneIntersection<T>({Line<T>({-3, -4}, {-3, -2})}, negativeHull);
        CHECK(clipped.area() == T(2) && clipped.loc({-4, -3}) == Location::ON);
        CHECK(::inter(hull, hull).area() == T(4));
        CHECK(::near(hull, hull).first == ::near(hull, hull).second);
    }
#endif

#if GEO2_TEST_LAYER >= 4
    Circle<T> circle({0, 0}, 1), other({2, 0}, 1);
    CHECK(::relation(circle, other) == CircleRelation::EXTERNAL_TANGENT);
    CHECK(::inter(circle, other, nullptr));
    CHECK(::inter(circle, hull, nullptr) && ::inter(hull, circle, nullptr));
    if constexpr (!std::is_integral_v<T>) {
        CHECK(::inter(circle, other).kind == HitKind::ONE);
        CHECK(::tangents(Point<T>{2, 0}, circle).lines.size() == 2);
        CHECK(::commonTangents(circle, other).lines.size() == 3);
        CHECK(::intersectionArea(circle, other) == T(0));
        CHECK(::intersectionArea(circle, hull) > T(0));
        CHECK(::near(hull, circle).first == ::near(hull, circle).second);
    }
#endif
}

#ifdef GEO2_TEST_MULTITU
int geometryPointWitness();
int geometryCircleWitness();
#endif

int main() {
    checkLayer<i64>();
    checkLayer<double>();
    checkLayer<Float>();
    checkLayer<FloatPointNumber<double>>();
    checkLayer<long double>();
    checkLayer<FloatPointNumber<long double>>();
#ifdef GEO2_TEST_MULTITU
    CHECK(geometryPointWitness() == 1 && geometryCircleWitness() == 4);
#endif
    std::cout << "Geo2 layer " << GEO2_TEST_LAYER << ": global API, scalar types, container access PASS\n";
}
