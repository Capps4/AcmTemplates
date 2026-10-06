#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/Geometry/Geo3/code.hpp"
#include "../../../Support/TestSupport.hpp"


using I = long long;
using Q = __int128_t;
using FP = Point<double>;
static_assert(!std::is_same_v<Point<int>, Vec<int>>);
static_assert(!std::is_same_v<Seg<int>, Line<int>>);
static_assert(std::is_same_v<decltype(Point<int>() - Point<int>()), Vec<int>>);
static_assert(std::is_same_v<decltype(Point<int>() + Vec<int>()), Point<int>>);
static_assert(std::is_same_v<decltype(Vec<int>() + Point<int>()), Point<int>>);

bool close(double a, double b) {
    return std::abs(a - b) <= 1E-9 * (1 + std::abs(a) + std::abs(b));
}

// Scalar determinant oracle, independent of Point<int>::cross/triple.
Q det(Point<int> a, Point<int> b, Point<int> c) {
    return Q(a.x) * b.y * c.z + Q(a.y) * b.z * c.x + Q(a.z) * b.x * c.y
         - Q(a.x) * b.z * c.y - Q(a.y) * b.x * c.z - Q(a.z) * b.y * c.x;
}

void vectors() {
    std::vector<Point<int>> ps;
    for (int x = -2; x <= 2; ++x)
        for (int y = -2; y <= 2; ++y)
            for (int z = -2; z <= 2; ++z)
                ps.emplace_back(x, y, z);
    for (auto a : ps) {
        for (auto b : ps) {
            CHECK(a.toVec().dot(b.toVec()) == I(a.x) * b.x + I(a.y) * b.y + I(a.z) * b.z);
            auto c = a.toVec().cross(b.toVec());
            CHECK(c.x == I(a.y) * b.z - I(a.z) * b.y);
            CHECK(c.y == I(a.z) * b.x - I(a.x) * b.z);
            CHECK(c.z == I(a.x) * b.y - I(a.y) * b.x);
            CHECK(c.dot(Vec<I>(a.x, a.y, a.z)) == 0);
            CHECK(a.to(b) + a == b);
        }
    }
    for (int i = 0; i < 32; ++i) {
        auto point = [] {
            return Point<int>(randomInt(-10000, 10000), randomInt(-10000, 10000),
                          randomInt(-10000, 10000));
        };
        auto a = point(), b = point(), c = point();
        CHECK(a.toVec().triple(b.toVec(), c.toVec()) == det(a, b, c));
    }
    Point<int> a(1000000000, -1000000000, 1000000000);
    Point<int> b(-1000000000, 1000000000, 1000000000);
    Point<int> c(1000000000, 1000000000, -1000000000);
    CHECK(a.toVec().triple(b.toVec(), c.toVec()) == det(a, b, c));
    CHECK(a.toVec().cross(b.toVec()).z == 0);
    CHECK(a.toVec().len2() == 3000000000000000000LL);
}

void lines() {
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                if (dx == 0 and dy == 0 and dz == 0)
                    continue;
                FP a(1, -2, 3), b(1 + dx, -2 + dy, 3 + dz);
                Line<double> line(a, b);
                auto from = Line<double>::fromVec(a, a.to(b));
                CHECK(from.p == line.p and from.v == line.v);
                Seg<double> seg(a, b);
                for (int x = -2; x <= 2; ++x) {
                    for (int y = -2; y <= 2; ++y) {
                        for (int z = -2; z <= 2; ++z) {
                            FP p(x, y, z);
                            double t = ((x - 1) * dx + (y + 2) * dy + (z - 3) * dz)
                                     / double(dx * dx + dy * dy + dz * dz);
                            FP q(1 + t * dx, -2 + t * dy, 3 + t * dz);
                            double dis = std::hypot(std::hypot(x - q.x, y - q.y), z - q.z);
                            CHECK(close(line.dist(p), dis));
                            CHECK(line.foot(p) == q);
                            CHECK((line.loc(p) == "ON") == close(dis, 0));
                            t = std::clamp(t, 0., 1.);
                            dis = std::hypot(std::hypot(x - 1 - t * dx, y + 2 - t * dy),
                                             z - 3 - t * dz);
                            CHECK(close(seg.dist(p), dis));
                            CHECK((seg.loc(p) == "ON") == close(dis, 0));
                        }
                    }
                }
            }
        }
    }
    Seg<int> one({1, 2, 3}, {1, 2, 3});
    CHECK(one.loc({1, 2, 3}) == "ON");
    CHECK(one.loc({1, 2, 4}) == "OUT");
    CHECK(one.dist({1, 2, 4}) == Float(1));
    Seg<int> zero;
    CHECK(zero.loc(Point<int>::O) == "ON");
}

void planes() {
    Plane<int> plane({0, 0, 2}, {1, 0, 2}, {0, 1, 2});
    for (int x = -3; x <= 3; ++x) {
        for (int y = -3; y <= 3; ++y) {
            for (int z = -3; z <= 3; ++z) {
                CHECK(plane.eval({x, y, z}) == z - 2);
                CHECK(plane.side({x, y, z}) == (z > 2) - (z < 2));
                CHECK(plane.dist({x, y, z}) == Float(std::abs(z - 2)));
            }
        }
    }
    auto hit = plane.relation(Line<int>({0, 0, 0}, {0, 0, 4}));
    CHECK(hit.type == "ONE" and hit.ps.size() == 1);
    CHECK(hit.ps[0].z == Float(2));
    CHECK(plane.relation(Line<int>({0, 0, 2}, {1, 1, 2})).type == "SAME");
    CHECK(plane.relation(Line<int>({0, 0, 1}, {1, 1, 1})).type == "NO");
    Plane<double> f({0, 0, 2}, {1, 0, 2}, {0, 1, 2});
    CHECK(f.foot({3, 4, 8}) == FP(3, 4, 2));
    Plane<int> slanted({1, 0, 0}, {0, 1, 0}, {0, 0, 1});
    CHECK(slanted.side({0, 0, 0}) == -1);
    CHECK(slanted.side({1, 1, 1}) == 1);
}

void solids() {
    Polyhedron<int> empty;
    CHECK(empty.area() == Float(0) and empty.vol6() == 0 and empty.vol() == Float(0));
    for (int side = 1; side <= 30; ++side) {
        Polyhedron<int> h;
        h.ps = {{0, 0, 0}, {side, 0, 0}, {0, side, 0}, {0, 0, side}};
        h.fs = {{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}};
        CHECK(h.vol6() == I(side) * side * side);
        CHECK(close(h.area().val(), side * side * (3 + std::sqrt(3.)) / 2));
        double vol = h.vol().val();
        h.reverse();
        CHECK(h.vol6() == -I(side) * side * side);
        CHECK(close(h.vol().val(), vol));
        for (auto &p : h.ps)
            p += Vec<int>(13, -7, 20);
        CHECK(h.vol6() == -I(side) * side * side);
        CHECK(close(h.vol().val(), vol));
    }
    std::stringstream io;
    Point<int> p;
    io << "1 2 3";
    io >> p;
    CHECK(p == Point<int>(1, 2, 3));
    std::ostringstream out;
    out << p;
    CHECK(out.str() == "(1, 2, 3)");
}

int coreCases() {
    vectors();
    lines();
    planes();
    solids();
    std::cout << "Geo3 vector/line/segment/plane/polyhedron correctness PASS; seed="
              << testSeed << '\n';
    return 0;
}

#include "../../../../src/Geometry/Geo3/code.hpp"
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
bool closeAdded(double a, double b) {
    return std::abs(a - b) <= 1e-8 * (1 + std::abs(a) + std::abs(b));
}

int run() {
    runCase("Geo3/translate", [] {
        Point<int> p(1, 2, 3);
        Vec<int> v(4, -3, 2);
        CHECK(p + v == Point<int>(5, -1, 5));
        CHECK((p + v) - p == v);
    });
    runCase("Geo3/dot-cross", [] {
        Vec<int> a(1, 2, 3), b(4, 5, 6);
        CHECK(a.dot(b) == 32);
        CHECK(a.cross(b) == Vec<long long>(-3, 6, -3));
    });
    runCase("Geo3/wide-triple", [] {
        Vec<int> a(1000000000, 0, 0), b(0, 1000000000, 0), c(0, 0, 1000000000);
        CHECK(a.triple(b, c) == __int128_t(1000000000) * 1000000000 * 1000000000);
    });
    runCase("Geo3/distance", [] {
        CHECK(Point<int>(0, 0, 0).dist2(Point<int>(1, 2, 2)) == 9);
    });
    runCase("Geo3/line-foot", [] {
        Line<double> l{{0, 0, 0}, {4, 0, 0}};
        CHECK(l.foot(Point<double>(2, 3, 4)) == Point<double>(2, 0, 0));
        CHECK(closeAdded(l.dist(Point<double>(2, 3, 4)), 5));
    });
    runCase("Geo3/segment-end", [] {
        Seg<double> s{{0, 0, 0}, {1, 0, 0}};
        CHECK(closeAdded(s.dist(Point<double>(2, 0, 0)), 1));
    });
    runCase("Geo3/zero-segment", [] {
        Seg<double> s{{2, 3, 4}, {2, 3, 4}};
        CHECK(closeAdded(s.dist(Point<double>(2, 3, 9)), 5));
    });
    runCase("Geo3/plane-foot", [] {
        Plane<double> p{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
        CHECK(p.foot(Point<double>(2, 3, 4)) == Point<double>(2, 3, 0));
        CHECK(closeAdded(p.dist(Point<double>(2, 3, 4)), 4));
    });
    runCase("Geo3/plane-line", [] {
        Plane<double> p{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
        CHECK(p.relation(Line<double>{{0, 0, 1}, {1, 0, 1}}).type == "NO");
        CHECK(p.relation(Line<double>{{0, 0, 0}, {1, 0, 0}}).type == "SAME");
    });
    runCase("Geo3/tetrahedron", [] {
        Polyhedron<double> p;
        p.ps = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        p.fs = {{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}};
        CHECK(closeAdded(p.vol(), 1. / 6));
        auto v = p.vol6();
        p.reverse();
        CHECK(p.vol6() == -v);
        CHECK(closeAdded(p.vol(), 1. / 6));
    });
    return 0;
}
}

int main() {
    runCase("Geo3/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
