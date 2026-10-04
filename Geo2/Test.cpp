#include "Circle.hpp"
#include "../TestSupport.hpp"
#include <limits>
using namespace _geo2;
using i64 = long long;
using Wrapped = FloatPointNumber<double>;
const char *stage = "edges";
int iteration = 0;

template <class T> double value(T x) { return double(x); }
template <> double value(Wrapped x) { return x.val(); }
bool approx(double a, double b, double tol = 1E-8) { return std::abs(a - b) <= tol * (1 + std::abs(a) + std::abs(b)); }
template <class T> bool close(Point<T> a, Point<T> b) { return approx(value(a.x), value(b.x)) && approx(value(a.y), value(b.y)); }

// Independent test accessors and scalar metric oracles, outside the public API.
template <class T> const auto &testVertices(const Convex<T> &h) { return h.vertices(); }
template <class T> const auto &testVertices(const Polygon<T> &p) { return p.ps; }
template <class Shape> auto testVertex(const Shape &s, int i) {
    int n = s.size(); return testVertices(s)[(i % n + n) % n];
}
template <class Shape> auto testEdge(const Shape &s, int i) {
    using P = std::decay_t<decltype(testVertex(s, i))>;
    using T = std::decay_t<decltype(P{}.x)>;
    return Seg<T>{testVertex(s, i), testVertex(s, i + 1)};
}
template <class T> double metricDistance(const Seg<T> &s, Point<T> p) {
    double dx = value(s.b.x) - value(s.a.x), dy = value(s.b.y) - value(s.a.y);
    double x = value(p.x) - value(s.a.x), y = value(p.y) - value(s.a.y);
    double d = dx * dx + dy * dy, t = d == 0 ? 0 : std::clamp((x * dx + y * dy) / d, 0., 1.);
    return std::hypot(x - dx * t, y - dy * t);
}
template <class T> double metricDistance(const Line<T> &l, Point<T> p) {
    double dx = value(l.v.x), dy = value(l.v.y);
    return std::abs(dx * (value(p.y) - value(l.p.y)) - dy * (value(p.x) - value(l.p.x))) / std::hypot(dx, dy);
}
template <class T> double metricDistance(const Seg<T> &a, const Seg<T> &b) {
    if (inter(a, b, nullptr)) return 0;
    return std::min({metricDistance(a, b.a), metricDistance(a, b.b), metricDistance(b, a.a), metricDistance(b, a.b)});
}
template <class A, class B> auto witnessDistance(const A &a, const B &b) {
    auto q = _geo2::near(a, b); return q.first.dist(q.second);
}

// Independent gift wrapping oracle, O(nh).
template <class T>
std::vector<Point<T>> jarvis(std::vector<Point<T>> p) {
    std::sort(p.begin(), p.end());
    p.erase(std::unique(p.begin(), p.end()), p.end());
    if (p.size() <= 2) return p;
    std::vector<Point<T>> h;
    int i = 0;
    do {
        h.push_back(p[i]);
        int j = (i + 1) % p.size();
        for (int k = 0; k < int(p.size()); ++k) {
            T c = p[i].cross(p[j], p[k]);
            if (c < 0 || (c == 0 && p[i].dist2(p[j]) < p[i].dist2(p[k]))) j = k;
        }
        i = j;
    } while (i);
    return h;
}

template <class T>
Convex<T> slowCut(const Convex<T> &h, const Line<T> &l) {
    std::vector<Point<T>> ps;
    if (h.size() <= 2) {
        for (auto p : h.vertices()) if (l.side(p) >= 0) ps.push_back(p);
        if (h.size() == 2) {
            T x = l.eval(h[0]), y = l.eval(h[1]);
            if (l.side(h[0]) * l.side(h[1]) < 0) ps.push_back(h[0] + (h[1] - h[0]) * (x / (x - y)));
        }
        return convexHull(std::move(ps));
    }
    for (int i = 0; i < h.size(); ++i) {
        auto a = h[i], b = testVertex(h, i + 1);
        T x = l.eval(a), y = l.eval(b);
        if (l.side(a) >= 0) ps.push_back(a);
        if (l.side(a) * l.side(b) < 0) ps.push_back(a + (b - a) * (x / (x - y)));
    }
    return convexHull(std::move(ps));
}

template <class T>
void sameHull(const Convex<T> &a, const Convex<T> &b) {
    if constexpr (std::is_integral_v<T>) CHECK(a.size() == b.size());
    if (a.empty() != b.empty()) {
        std::cerr << "empty mismatch stage=" << stage << " rep=" << iteration << " actual:";
        for (auto p : a.vertices()) std::cerr << p;
        std::cerr << " expected:";
        for (auto p : b.vertices()) std::cerr << p;
        std::cerr << "\n";
    }
    CHECK(a.empty() == b.empty());
    if (!approx(value(a.area()), value(b.area()))) {
        std::cerr << "area stage=" << stage << " rep=" << iteration << " actual=" << value(a.area()) << " expected=" << value(b.area()) << "\n";
    }
    CHECK(approx(value(a.area()), value(b.area())));
    // A corner may be slightly reordered by floating roundoff.
    for (auto p : a.vertices()) {
        bool found = false;
        for (auto q : b.vertices()) found |= close(p, q);
        for (int i = 0; i < b.size(); ++i) found |= approx(value(metricDistance(testEdge(b, i), p)), 0);
        if (!found) {
            std::cerr << "stage=" << stage << " rep=" << iteration << " unmatched=" << p << " actual:";
            for (auto q : a.vertices()) std::cerr << q;
            std::cerr << " expected:";
            for (auto q : b.vertices()) std::cerr << q;
            std::cerr << "\n";
        }
        CHECK(found);
    }
    for (auto p : b.vertices()) {
        bool found = false;
        for (auto q : a.vertices()) found |= close(p, q);
        for (int i = 0; i < a.size(); ++i) found |= approx(value(metricDistance(testEdge(a, i), p)), 0);
        if (!found) {
            std::cerr << "stage=" << stage << " rep=" << iteration << " expected vertex=" << p << " actual:";
            for (auto q : a.vertices()) std::cerr << q;
            std::cerr << " expected:";
            for (auto q : b.vertices()) std::cerr << q;
            std::cerr << "\n";
        }
        CHECK(found);
    }
}

template <class T>
void predicates(int rounds) {
    for (int rep = 0; rep < rounds; ++rep) {
        std::vector<Point<T>> ps;
        for (int i = randomInt(0, 40); i--;) ps.emplace_back(T(randomInt(-20, 20)), T(randomInt(-20, 20)));
        auto h = convexHull(ps);
        CHECK(h.vertices() == jarvis(ps));
        if (h.empty()) { CHECK(h.loc({0, 0}) == Location::OUT); continue; }
        auto reversed = h.vertices();
        std::reverse(reversed.begin(), reversed.end());
        sameHull(h, Convex<T>::fromBoundary(reversed));
        T maxd = 0;
        for (auto a : h.vertices()) for (auto b : h.vertices()) maxd = std::max(maxd, a.dist2(b));
        auto pair = farthestPair(h);
        CHECK(pair && h[(*pair)[0]].dist2(h[(*pair)[1]]) == maxd);
        for (int k = 0; k < 30; ++k) {
            Point<T> p(T(randomInt(-30, 30)), T(randomInt(-30, 30)));
            CHECK(h.loc(p) == h.polygon().loc(p));
            Vec<T> v(T(randomInt(-20, 20)), T(randomInt(-20, 20)));
            if (v == Vec<T>::O) v.x = 1;
            auto l = Line<T>::fromVec(p, v);
            bool brute = false;
            for (int i = 0; i < h.size(); ++i) brute |= inter(l, testEdge(h, i), nullptr);
            CHECK(inter(h, l, nullptr) == brute);
            Seg<T> s{p, p + v};
            bool bs = h.loc(s.a) != Location::OUT || h.loc(s.b) != Location::OUT;
            for (int i = 0; i < h.size(); ++i) bs |= inter(s, testEdge(h, i), nullptr);
            CHECK(inter(h, s, nullptr) == bs);
            if (h.loc(p) == Location::OUT) {
                auto ts = h.tangents(p);
                for (int i : ts) {
                    bool pos = false, neg = false;
                    for (auto q : h.vertices()) { int c = orient(p, h[i], q); pos |= c > 0; neg |= c < 0; }
                    if (pos && neg) {
                        std::cerr << "tangent failure p=" << p << " index=" << i << " hull:";
                        for (auto q : h.vertices()) std::cerr << q;
                        std::cerr << '\n';
                    }
                    CHECK(!(pos && neg));
                }
            }
        }
        std::vector<Point<T>> other;
        for (int i = randomInt(1, 15); i--;) other.emplace_back(T(randomInt(-20, 20)), T(randomInt(-20, 20)));
        auto b = convexHull(other);
        std::vector<Point<T>> sums;
        for (auto p : h.vertices()) for (auto q : b.vertices()) sums.push_back(p + q.toVec());
        stage = "minkowski"; iteration = rep;
        sameHull(minkowskiSum(h, b), convexHull(sums));
        bool brute = false;
        for (auto p : h.vertices()) brute |= b.loc(p) != Location::OUT;
        for (auto p : b.vertices()) brute |= h.loc(p) != Location::OUT;
        for (int i = 0; i < h.size(); ++i) for (int j = 0; j < b.size(); ++j) brute |= inter(testEdge(h, i), testEdge(b, j), nullptr);
        CHECK(inter(h, b, nullptr) == brute);
        if constexpr (!std::is_integral_v<T>) {
        auto witnesses = _geo2::near(h, b);
        auto belongs = [](const Convex<T> &h, Point<T> p) {
            if (h.loc(p) != Location::OUT) return true;
            for (int i = 0; i < h.size(); ++i) if (metricDistance(testEdge(h, i), p) < 1E-8) return true;
            return false;
        };
        CHECK(belongs(h, witnesses.first) && belongs(b, witnesses.second));
        if (!brute) {
            double d = value(h[0].dist(b[0]));
            for (int i = 0; i < h.size(); ++i) for (int j = 0; j < b.size(); ++j) d = std::min(d, metricDistance(testEdge(h, i), testEdge(b, j)));
            CHECK(approx(value(witnessDistance(h, b)), value(d)));
        } else CHECK(approx(value(witnessDistance(h, b)), 0));
        }
    }
}

template <class T>
void constructions(int rounds) {
    auto domain = convexHull<T>({{-100, -100}, {100, -100}, {100, 100}, {-100, 100}});
    for (int rep = 0; rep < rounds; ++rep) {
        std::vector<Point<T>> ps;
        for (int i = randomInt(1, 25); i--;) ps.emplace_back(T(randomInt(-30, 30)), T(randomInt(-30, 30)));
        auto h = convexHull(ps);
        auto rect = minBoundingRect(h);
        CHECK(rect.has_value());
        if (h.size() > 2) {
            double width = 1E100, area = 1E100;
            for (int i = 0; i < h.size(); ++i) {
                auto v = testEdge(h, i).vec();
                double lo = 1E100, hi = -1E100, top = 0;
                for (auto p : h.vertices()) {
                    double dot = value(v.dot(p - h[i]));
                    lo = std::min(lo, dot); hi = std::max(hi, dot);
                    top = std::max(top, value(v.cross(p - h[i])));
                }
                width = std::min(width, top / value(v.len()));
                area = std::min(area, (hi - lo) * top / value(v.len2()));
            }
            CHECK(approx(value(minWidth(h)), width));
            CHECK(approx(value(rect->area), area));
            CHECK(approx(value(Polygon<T>{{rect->ps.begin(), rect->ps.end()}}.area()), area));
        }
        std::vector<Line<T>> lines;
        for (int k = 0; k < 15; ++k) {
            Point<T> p(T(randomInt(-40, 40)), T(randomInt(-40, 40)));
            Vec<T> v(T(randomInt(-30, 30)), T(randomInt(-30, 30)));
            if (v == Vec<T>::O) v.x = 1;
            auto l = Line<T>::fromVec(p, v);
            stage = "cut"; iteration = rep;
            auto fast = cutLeft(h, l), slow = slowCut(h, l);
            if (!approx(value(fast.area()), value(slow.area()))) {
                std::cerr << "line=" << l.p << " dir=" << l.v << " hull:";
                for (auto q : h.vertices()) std::cerr << q;
                std::cerr << "\n";
            }
            sameHull(fast, slow);
            auto hit = inter(h, l);
            auto pieces = inter(h.polygon(), l);
            CHECK(bool(hit) == !pieces.empty());
            if (hit) {
                CHECK(pieces.size() == 1);
                CHECK(close(hit.ps[0], pieces[0].a));
                CHECK(close(hit.size() == 1 ? hit.ps[0] : hit.ps[1], pieces[0].b));
            }
            lines.push_back(l);
        }
        auto slow = domain;
        for (auto l : lines) slow = slowCut(slow, l);
        stage = "halfPlane"; iteration = rep;
        auto hp = halfPlaneIntersection(lines, domain);
        bool roundedDegeneracy = false;
        if constexpr (std::is_floating_point_v<T>) {
            if (hp.empty() != slow.empty()) {
                const auto &candidate = hp.empty() ? slow : hp;
                CHECK(candidate.size() <= 2);
                for (auto p : candidate.vertices()) {
                    for (auto l : lines) CHECK(value(l.eval(p)) >= -1E-8);
                    for (int i = 0; i < domain.size(); ++i) CHECK(value(testEdge(domain, i).line().eval(p)) >= -1E-8);
                }
                roundedDegeneracy = true;
            }
        }
        if (!roundedDegeneracy) sameHull(hp, slow);
        auto b = convexHull<T>({{-10, -10}, {20, -10}, {20, 15}, {-10, 15}});
        auto clipped = h;
        for (int i = 0; i < b.size(); ++i) clipped = slowCut(clipped, testEdge(b, i).line());
        stage = "convex intersect"; iteration = rep;
        sameHull(inter(h, b), clipped);
        Circle<T> a({T(randomInt(-10, 10)), T(randomInt(-10, 10))}, T(randomInt(0, 15)));
        Circle<T> c({T(randomInt(-10, 10)), T(randomInt(-10, 10))}, T(randomInt(0, 15)));
        auto hit = inter(a, c);
        for (int i = 0; i < hit.size(); ++i) {
            CHECK(approx(value(hit.ps[i].dist2(a.o)), value(a.r * a.r)));
            CHECK(approx(value(hit.ps[i].dist2(c.o)), value(c.r * c.r)));
        }
        auto ts = commonTangents(a, c);
        for (auto l : ts.lines) {
            CHECK(approx(value(metricDistance(l, a.o)), value(a.r)));
            CHECK(approx(value(metricDistance(l, c.o)), value(c.r)));
        }
        T ia = intersectionArea(a, c);
        CHECK(approx(value(ia), value(intersectionArea(c, a))));
        CHECK(value(ia) >= -1E-9 && value(ia) <= std::min(value(a.area()), value(c.area())) + 1E-8);
        T pa = intersectionArea(a, h.polygon());
        CHECK(value(pa) >= 0 && value(pa) <= std::min(value(a.area()), value(h.area())) + 1E-7);
        auto rev = h.polygon();
        std::reverse(rev.ps.begin(), rev.ps.end());
        CHECK(approx(value(pa), value(intersectionArea(a, rev))));
        auto contacts = tangents(Point<T>(30, 40), a);
        for (auto l : contacts.lines) {
            CHECK(approx(value(metricDistance(l, a.o)), value(a.r)));
            CHECK(approx(value(metricDistance(l, {30, 40})), 0));
        }
    }
}

template <class T>
void edges() {
    CHECK(approx(value(Polygon<T>{{{0, 0}, {1, 0}, {0, 1}}}.area()), .5));
    auto h = convexHull<T>({{0, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}, {2, 0}});
    CHECK(h.size() == 4);
    auto degen = Convex<T>::fromBoundary({{3, 0}, {2, 0}, {1, 0}, {3, 0}});
    CHECK(degen.size() == 2 && degen[0] == Point<T>(1, 0));
    for (int x = -2; x <= 6; ++x) for (int y = -2; y <= 6; ++y) {
        Point<T> p{T(x), T(y)};
        if (h.loc(p) == Location::OUT) {
            for (int t : h.tangents(p)) {
                bool pos = false, neg = false;
                for (auto q : h.vertices()) { auto c = orient(p, h[t], q); pos |= c > 0; neg |= c < 0; }
                CHECK(!pos || !neg);
            }
        }
        for (int u = -2; u <= 6; ++u) for (int v = -2; v <= 6; ++v) {
            if (x == u && y == v) continue;
            Line<T> l(p, {T(u), T(v)});
            bool brute = false;
            for (int i = 0; i < 4; ++i) brute |= inter(l, testEdge(h, i), nullptr);
            CHECK(inter(h, l, nullptr) == brute);
        }
    }
    CHECK(approx(metricDistance(Seg<T>{{0, 0}, {3, 0}}, Point<T>{1, 2}), 2));
    CHECK(approx(metricDistance(Seg<T>{{0, 0}, {0, 0}}, Point<T>{3, 4}), 5));
    CHECK(!farthestPair(Convex<T>{}));
    if constexpr (!std::is_integral_v<T>) {
        auto vertexHull = convexHull<T>({{-27, 13}, {-6, -19}, {3, -30}, {9, -28}, {29, 0}, {17, 14}, {5, 21}, {-16, 26}, {-19, 23}});
        Line<T> vertexLine({-12, 21}, {-24, 21});
        sameHull(cutLeft(vertexHull, vertexLine), slowCut(vertexHull, vertexLine));
        CHECK(inter(vertexLine, Seg<T>{{5, 21}, {-16, 26}}).ps[0] == Point<T>(5, 21));
        Polygon<T> concave{{{0, 0}, {6, 0}, {6, 6}, {4, 6}, {4, 2}, {2, 2}, {2, 6}, {0, 6}}};
        CHECK(!concave.isConvex());
        auto pieces = inter(concave, Line<T>({-1, 3}, {7, 3}));
        CHECK(pieces.size() == 2 && close(pieces[0].a, Point<T>(0, 3)) && close(pieces[0].b, Point<T>(2, 3)));
        CHECK(close(pieces[1].a, Point<T>(4, 3)) && close(pieces[1].b, Point<T>(6, 3)));
        CHECK(close(h.polygon().centroid(), Point<T>(2, 2)));
        auto segment = halfPlaneIntersection<T>({Line<T>({0, 0}, {0, 1}), Line<T>({0, 1}, {0, 0})}, h);
        sameHull(segment, convexHull<T>({{0, 0}, {0, 4}}));
        auto point = halfPlaneIntersection<T>({Line<T>({0, 0}, {0, 1}), Line<T>({1, 0}, {0, 0})}, h);
        CHECK(point.size() == 1 && close(point[0], Point<T>(0, 0)));
        CHECK(halfPlaneIntersection<T>({Line<T>({-1, 0}, {-1, 1})}, h).empty());
        Circle<T> c({0, 0}, 1);
        CHECK(inter(c, Line<T>({0, 1}, {1, 1})).kind == HitKind::ONE);
        CHECK(inter(c, Seg<T>{{0, 1}, {0, 1}}).kind == HitKind::ONE);
        CHECK(inter(c, Circle<T>({0, 0}, 1)).kind == HitKind::CO);
        CHECK(inter(Circle<T>({0, 0}, 0), Circle<T>({0, 0}, 0)).kind == HitKind::ONE);
        CHECK(commonTangents(c, c).infinite);
        CHECK(tangents(Point<T>{0, 0}, Circle<T>({0, 0}, 0)).infinite);
        CHECK(commonTangents(c, Circle<T>({2, 0}, 1)).lines.size() == 3);
        CHECK(commonTangents(c, Circle<T>({3, 0}, 1)).lines.size() == 4);
        CHECK(commonTangents(c, Circle<T>({0, 0}, 2)).lines.empty());
        auto full = convexHull<T>({{-2, -2}, {2, -2}, {2, 2}, {-2, 2}}).polygon();
        CHECK(approx(value(intersectionArea(c, full)), std::acos(-1.)));
        CHECK(approx(value(intersectionArea(c, h.polygon())), std::acos(-1.) / 4));
        CHECK(approx(value(intersectionArea(Circle<T>({2, 2}, 100), h.polygon())), 16));
        CHECK(inter(c, Seg<T>{{-2, 0}, {0, 0}}).size() == 1);
        CHECK(inter(c, Seg<T>{{-2, 0}, {2, 0}}).size() == 2);
        CHECK(inter(c, Seg<T>{{2, 0}, {3, 0}}).size() == 0);
        for (int i = 0; i < h.size(); ++i) {
            auto l = testEdge(h, i).line();
            sameHull(cutLeft(h, l), h);
            CHECK(cutLeft(h, l.reversed()).size() == 2);
        }
    }
}

// Analytic disk/axis-aligned rectangle oracle, independent of edge sectors.
double diskCorner(double x, double y, double r) {
    if (!r) return 0;
    double sign = x * y < 0 ? -1 : 1;
    x = std::min(std::abs(x), r); y = std::min(std::abs(y), r);
    double t = std::min(x, std::sqrt(std::max(0., r * r - y * y)));
    auto primitive = [&](double q) { return (q * std::sqrt(std::max(0., r * r - q * q)) + r * r * std::asin(q / r)) / 2; };
    return sign * (t * y + primitive(x) - primitive(t));
}

template <class T>
void areaOracle() {
    for (int rep = 0; rep < 3000; ++rep) {
        double x0 = randomInt(-30, 30), x1 = randomInt(-30, 30);
        double y0 = randomInt(-30, 30), y1 = randomInt(-30, 30), r = randomInt(0, 25);
        if (x0 > x1) std::swap(x0, x1);
        if (y0 > y1) std::swap(y0, y1);
        Polygon<T> p{{{T(x0), T(y0)}, {T(x1), T(y0)}, {T(x1), T(y1)}, {T(x0), T(y1)}}};
        double expected = diskCorner(x1, y1, r) - diskCorner(x0, y1, r) - diskCorner(x1, y0, r) + diskCorner(x0, y0, r);
        CHECK(approx(value(intersectionArea(Circle<T>({0, 0}, T(r)), p)), expected));
    }
}

template <class T>
void boundedHalfPlanes() {
    auto domain = convexHull<T>({{-100, -100}, {100, -100}, {100, 100}, {-100, 100}});
    for (int rep = 0; rep < 1000; ++rep) {
        std::vector<Line<T>> lines;
        for (int i = 0; i < 30; ++i) {
            Vec<T> v{T(randomInt(-10, 10)), T(randomInt(-10, 10))};
            if (v == Vec<T>::O) v.x = 1;
            lines.push_back(Line<T>::fromVec(Point<T>::O - v.rot90() * T(randomInt(1, 5)), v));
        }
        auto expected = domain;
        for (auto l : lines) expected = slowCut(expected, l);
        stage = "bounded halfplanes"; iteration = rep;
        auto actual = halfPlaneIntersection(lines, domain);
        if (!approx(value(actual.area()), value(expected.area()))) {
            std::cerr << "wrapped=" << !std::is_floating_point_v<T> << " lines:";
            for (auto l : lines) std::cerr << l.p << l.v;
            std::cerr << " actual:"; for (auto q : actual.vertices()) std::cerr << q;
            std::cerr << " expected:"; for (auto q : expected.vertices()) std::cerr << q;
            std::cerr << "\n";
        }
        sameHull(actual, expected);
    }
}

void segmentOracle() {
    std::vector<Point<i64>> points;
    for (i64 x = -1; x <= 1; ++x) for (i64 y = -1; y <= 1; ++y) points.emplace_back(x, y);
    for (auto a : points) for (auto b : points) for (auto c : points) for (auto d : points) {
        auto contains = [](Point<i64> a, Point<i64> b, Point<i64> p) {
            return a.cross(b, p) == 0 && std::min(a.x, b.x) <= p.x && p.x <= std::max(a.x, b.x)
                 && std::min(a.y, b.y) <= p.y && p.y <= std::max(a.y, b.y);
        };
        i64 x = a.cross(b, c), y = a.cross(b, d), u = c.cross(d, a), v = c.cross(d, b);
        bool expected = (x * y < 0 && u * v < 0) || contains(a, b, c) || contains(a, b, d) || contains(c, d, a) || contains(c, d, b);
        CHECK(inter(Seg<i64>{a, b}, Seg<i64>{c, d}, nullptr) == expected);
        auto p = [](Point<i64> a) { return Point<Wrapped>(double(a.x), double(a.y)); };
        auto hit = inter(Seg<Wrapped>{p(a), p(b)}, Seg<Wrapped>{p(c), p(d)});
        CHECK(bool(hit) == expected);
        for (int i = 0; i < hit.size(); ++i) {
            CHECK(Seg<Wrapped>{p(a), p(b)}.loc(hit.ps[i]) == Location::ON);
            CHECK(Seg<Wrapped>{p(c), p(d)}.loc(hit.ps[i]) == Location::ON);
        }
    }
    using W = Wrapped;
    CHECK(Line<W>({0, 0}, {1, 0}).side({0, W(5E-13)}) == 0);
    CHECK(Line<double>({0, 0}, {1, 0}).side({0, 5E-13}) == 1);
    auto l = Line<double>::fromVec({1E20, 1E20}, {1, 0});
    CHECK(l.v == Vec<double>(1, 0));
    static_assert(sizeof(Point<W>) == 2 * sizeof(W));
    static_assert(sizeof(Vec<i64>) == 2 * sizeof(i64));
    static_assert(std::is_same_v<decltype(Circle<W>({0, 0}, 1).area()), W>);
    static_assert(!std::is_convertible_v<W, double>);
}

void largeHull() {
    constexpr int n = 65536;
    std::vector<Point<double>> ps;
    for (int i = 0; i < n; ++i) {
        double a = 2 * std::acos(-1.) * i / n;
        ps.emplace_back(100 * std::cos(a), 100 * std::sin(a));
    }
    auto h = Convex<double>::fromBoundary(ps);
    CHECK(h.size() == n);
    CHECK(approx(diameter(h), 200));
    CHECK(approx(minWidth(h), 200, 1E-7));
    CHECK(approx(minBoundingRect(h)->area, 40000, 1E-7));
    CHECK(approx(minkowskiSum(h, h).area(), 4 * h.area()));
    for (int i = 0; i < 80; ++i) {
        double a = i * 2.399963229728653;
        Vec<double> v{std::cos(a), std::sin(a)};
        auto hit = inter(h, Line<double>::fromVec({0, 0}, v));
        CHECK(hit.kind == HitKind::SEG);
        CHECK(approx(hit.ps[0].dist(Point<double>::O), 100, 1E-7));
        CHECK(approx(hit.ps[1].dist(Point<double>::O), 100, 1E-7));
        Point<double> p = Point<double>::O + v * 200;
        for (int t : h.tangents(p)) {
            double lo = 1E100, hi = -1E100;
            for (auto q : h.vertices()) {
                double c = p.cross(h[t], q);
                lo = std::min(lo, c); hi = std::max(hi, c);
            }
            CHECK(lo >= -1E-8 || hi <= 1E-8);
        }
    }
    double circleArea = intersectionArea(Circle<double>({0, 0}, 100), Circle<double>({30, 20}, 100));
    // Equal-direction dense edges are intentionally tested with tolerant scalars.
    std::vector<Point<Wrapped>> wp, shifted;
    for (auto p : ps) {
        wp.emplace_back(p.x, p.y);
        shifted.emplace_back(p.x + 30, p.y + 20);
    }
    auto wh = Convex<Wrapped>::fromBoundary(wp), wb = Convex<Wrapped>::fromBoundary(shifted);
    CHECK(approx(value(inter(wh, wb).area()), circleArea, 1E-7));
    // Native double uses separated edge directions in this conditioning test.
    for (int i = 0; i < n; ++i) {
        double a = 2 * std::acos(-1.) * (i + .37) / n;
        ps[i] = {100 * std::cos(a) + 30, 100 * std::sin(a) + 20};
    }
    auto b = Convex<double>::fromBoundary(ps);
    CHECK(approx(inter(h, b).area(), circleArea, 1E-7));
    // Many cyclic vertices with an integral, asymmetric boundary.
    std::vector<Point<i64>> parabola;
    for (i64 i = -4096; i <= 4096; ++i) parabola.emplace_back(i, i * i);
    auto integerHull = convexHull(parabola);
    CHECK(integerHull.size() == 8193);
    for (i64 x = -12; x <= 12; ++x) for (i64 y = -12; y <= 12; ++y) {
        if (!x && !y) continue;
        Vec<i64> v{x, y};
        auto line = Line<i64>::fromVec({0, 0}, v);
        bool expected = false;
        for (int i = 0; i < integerHull.size(); ++i) expected |= inter(testEdge(integerHull, i), line, nullptr);
        CHECK(inter(integerHull, line, nullptr) == expected);
    }
}

// The distance from a circle to a connected set is its radius's distance
// to the set's minimum/maximum radial interval; this oracle constructs no hits.
double radialGap(double radius, double low, double high) {
    return std::max({low - radius, radius - high, 0.});
}
template <class T, class Shape>
void verifyWitness(const Shape &shape, const Circle<T> &c, double expected) {
    auto [p, q] = _geo2::near(shape, c);
    auto reverse = _geo2::near(c, shape);
    if (!approx(value(p.dist(q)), expected)) {
        std::cerr << "nearest expected=" << expected << " actual=" << value(p.dist(q)) << " c=" << c.o << " r=" << c.r << " points=" << p << q << " segment=" << std::is_same_v<Shape, Seg<T>> << " line=" << std::is_same_v<Shape, Line<T>> << "\n";
        if constexpr (std::is_same_v<Shape, Seg<T>>) std::cerr << "segment=" << shape.a << shape.b << "\n";
    }
    CHECK(approx(value(p.dist(q)), expected));
    CHECK(approx(value(reverse.first.dist(reverse.second)), expected));
    CHECK(approx(value(c.o.dist2(q)), value(c.r * c.r)));
    CHECK(approx(value(c.o.dist2(reverse.first)), value(c.r * c.r)));
    CHECK(inter(shape, c, nullptr) == (expected < 1E-9));
}

template <class T>
void nearestOracles(int rounds) {
    for (int rep = 0; rep < rounds; ++rep) {
        auto point = [] { return Point<T>{T(randomInt(-20, 20)), T(randomInt(-20, 20))}; };
        Circle<T> c(point(), T(randomInt(0, 12))), d(point(), T(randomInt(0, 12)));
        Seg<T> s{point(), point()}, t{point(), point()};
        double lo = metricDistance(s, c.o), hi = std::max(value(s.a.dist(c.o)), value(s.b.dist(c.o)));
        verifyWitness(s, c, radialGap(value(c.r), lo, hi));
        auto ns = _geo2::near(s, c);
        CHECK(metricDistance(s, ns.first) < 1E-7);
        double centerDistance = value(c.o.dist(d.o));
        double circleDistance = std::max({centerDistance - value(c.r + d.r), value(std::abs(c.r - d.r)) - centerDistance, 0.});
        verifyWitness(d, c, circleDistance);
        CHECK(approx(value(_geo2::near(d, c).first.dist2(d.o)), value(d.r * d.r)));
        auto ss = _geo2::near(s, t);
        CHECK(approx(value(ss.first.dist(ss.second)), metricDistance(s, t)));
        CHECK(metricDistance(s, ss.first) < 1E-7 && metricDistance(t, ss.second) < 1E-7);
        CHECK(approx(value(witnessDistance(t, s)), metricDistance(s, t)));
        auto p = point();
        auto sp = _geo2::near(p, s);
        CHECK(sp.first == p && metricDistance(s, sp.second) < 1E-7);
        CHECK(approx(value(sp.first.dist(sp.second)), metricDistance(s, p)));
        if (t.a != t.b) {
            auto l = t.line();
            auto lp = _geo2::near(p, l);
            CHECK(lp.first == p && metricDistance(l, lp.second) < 1E-7);
            CHECK(approx(value(lp.first.dist(lp.second)), metricDistance(l, p)));
            verifyWitness(l, c, std::max(metricDistance(l, c.o) - value(c.r), 0.));
            double expected = inter(l, s, nullptr) ? 0 : std::min(metricDistance(l, s.a), metricDistance(l, s.b));
            auto ls = _geo2::near(l, s);
            CHECK(approx(value(ls.first.dist(ls.second)), expected));
            CHECK(metricDistance(l, ls.first) < 1E-7 && metricDistance(s, ls.second) < 1E-7);
        }
        // Both convex and concave regions, including point/segment hulls.
        std::vector<Point<T>> ps;
        for (int i = randomInt(1, 15); i--;) ps.push_back(point());
        auto h = convexHull(ps);
        Polygon<T> concave{{{0, 0}, {6, 0}, {6, 6}, {4, 6}, {4, 2}, {2, 2}, {2, 6}, {0, 6}}};
        auto region = [&](const auto &shape) {
            double low = shape.loc(c.o) == Location::OUT ? 1E100 : 0, high = 0;
            for (auto p : testVertices(shape)) high = std::max(high, value(c.o.dist(p)));
            for (int i = 0; i < shape.size(); ++i) low = std::min(low, metricDistance(testEdge(shape, i), c.o));
            double expected = radialGap(value(c.r), low, high);
            verifyWitness(shape, c, expected);
            auto q = _geo2::near(shape, c);
            bool valid = shape.loc(q.first) != Location::OUT;
            for (int i = 0; i < shape.size(); ++i) valid |= metricDistance(testEdge(shape, i), q.first) < 1E-7;
            CHECK(valid);
        };
        region(h); region(concave);
        auto hc = _geo2::near(h, concave);
        double expected = h.loc(concave.ps[0]) != Location::OUT || concave.loc(h[0]) != Location::OUT ? 0 : 1E100;
        for (int i = 0; i < h.size(); ++i) for (int j = 0; j < concave.size(); ++j)
            expected = std::min(expected, metricDistance(testEdge(h, i), testEdge(concave, j)));
        CHECK(approx(value(hc.first.dist(hc.second)), expected));
    }
}

void circlePredicateOracle() {
    for (int rep = 0; rep < 20000; ++rep) {
        auto point = [] { return Point<i64>{randomInt(-10, 10), randomInt(-10, 10)}; };
        Circle<i64> c(point(), randomInt(0, 10)), d(point(), randomInt(0, 10));
        Seg<i64> s{point(), point()};
        double low = metricDistance(s, c.o), high = std::max(s.a.dist(c.o), s.b.dist(c.o));
        CHECK(inter(c, s, nullptr) == (radialGap(c.r, low, high) < 1E-9));
        double distance = c.o.dist(d.o);
        CHECK(inter(c, d, nullptr) == (std::abs(c.r - d.r) <= distance && distance <= c.r + d.r));
    }
}

// Regression: nearly duplicate intersections must retain a half-plane corner.
template <class T>
void halfPlaneCornerRegression() {
    const std::array<int, 4> data[] = {
        {36,-24,6,9}, {-16,0,0,-8}, {40,35,-7,8}, {-25,20,-4,-5}, {-30,30,-6,-6},
        {0,-50,10,0}, {-1,9,-9,-1}, {50,-50,10,10}, {5,45,-9,1}, {25,15,-3,5},
        {8,-28,7,2}, {-27,-30,10,-9}, {30,-25,5,6}, {-25,-10,2,-5}, {-15,9,-3,-5},
        {30,-3,1,10}, {-4,-8,4,-2}, {-10,1,-1,-10}, {0,12,-6,0}, {12,-20,5,3},
        {-15,12,-4,-5}, {10,-1,1,10}, {-4,-8,8,-4}, {-10,0,0,-5}, {21,21,-7,7},
        {-8,4,-4,-8}, {4,-12,6,2}, {-9,3,-3,-9}, {16,18,-9,8}, {0,4,-4,0}
    };
    std::vector<Line<T>> lines;
    for (auto d : data) lines.push_back(Line<T>::fromVec({T(d[0]), T(d[1])}, {T(d[2]), T(d[3])}));
    auto domain = convexHull<T>({{-100,-100}, {100,-100}, {100,100}, {-100,100}});
    auto expected = domain;
    for (auto l : lines) expected = slowCut(expected, l);
    auto actual = halfPlaneIntersection(lines, domain);
    sameHull(actual, expected);
    CHECK(actual.loc({-10, 0}) == Location::ON);
}

int main() {
    halfPlaneCornerRegression<double>(); halfPlaneCornerRegression<Wrapped>();
    nearestOracles<double>(3000); nearestOracles<Wrapped>(3000);
    circlePredicateOracle();
    largeHull();
    segmentOracle();
    areaOracle<double>(); areaOracle<Wrapped>();
    boundedHalfPlanes<double>(); boundedHalfPlanes<Wrapped>();
    edges<i64>(); edges<double>(); edges<Wrapped>();
    predicates<i64>(2000); predicates<double>(2000); predicates<Wrapped>(2000);
    constructions<double>(2000); constructions<Wrapped>(2000);
    std::cout << "Geo2: 65536-vertex hull, 6561 segment pairs, 6000 area oracles, 2000 bounded HPI, 6000 predicate + 4000 construction rounds, 6000 nearest-point + 20000 circle-predicate oracles PASS; seed=" << testSeed << '\n';
}
