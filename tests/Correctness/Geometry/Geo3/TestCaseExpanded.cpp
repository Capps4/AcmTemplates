#include "../../../../src/Geometry/Geo3/code.hpp"
#include "../../../Support/CaseSupport.hpp"
bool closeAdded(double a, double b) {
    return std::abs(a - b) <= 1e-8 * (1 + std::abs(a) + std::abs(b));
}

int main() {
    runCase("Geo3/01-translate", [] {
        Point<int> p(1, 2, 3);
        Vec<int> v(4, -3, 2);
        CHECK(p + v == Point<int>(5, -1, 5));
        CHECK((p + v) - p == v);
    });
    runCase("Geo3/02-dot-cross", [] {
        Vec<int> a(1, 2, 3), b(4, 5, 6);
        CHECK(a.dot(b) == 32);
        CHECK(a.cross(b) == Vec<long long>(-3, 6, -3));
    });
    runCase("Geo3/03-wide-triple", [] {
        Vec<int> a(1000000000, 0, 0), b(0, 1000000000, 0), c(0, 0, 1000000000);
        CHECK(a.triple(b, c) == __int128_t(1000000000) * 1000000000 * 1000000000);
    });
    runCase("Geo3/04-distance", [] {
        CHECK(Point<int>(0, 0, 0).dist2(Point<int>(1, 2, 2)) == 9);
    });
    runCase("Geo3/05-line-foot", [] {
        Line<double> l{{0, 0, 0}, {4, 0, 0}};
        CHECK(l.foot(Point<double>(2, 3, 4)) == Point<double>(2, 0, 0));
        CHECK(closeAdded(l.dist(Point<double>(2, 3, 4)), 5));
    });
    runCase("Geo3/06-segment-end", [] {
        Seg<double> s{{0, 0, 0}, {1, 0, 0}};
        CHECK(closeAdded(s.dist(Point<double>(2, 0, 0)), 1));
    });
    runCase("Geo3/07-zero-segment", [] {
        Seg<double> s{{2, 3, 4}, {2, 3, 4}};
        CHECK(closeAdded(s.dist(Point<double>(2, 3, 9)), 5));
    });
    runCase("Geo3/08-plane-foot", [] {
        Plane<double> p{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
        CHECK(p.foot(Point<double>(2, 3, 4)) == Point<double>(2, 3, 0));
        CHECK(closeAdded(p.dist(Point<double>(2, 3, 4)), 4));
    });
    runCase("Geo3/09-plane-line", [] {
        Plane<double> p{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
        CHECK(p.relation(Line<double>{{0, 0, 1}, {1, 0, 1}}).type == "NO");
        CHECK(p.relation(Line<double>{{0, 0, 0}, {1, 0, 0}}).type == "SAME");
    });
    runCase("Geo3/10-tetrahedron", [] {
        Polyhedron<double> p;
        p.ps = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        p.fs = {{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}};
        CHECK(closeAdded(p.vol(), 1. / 6));
        auto v = p.vol6();
        p.reverse();
        CHECK(p.vol6() == -v);
        CHECK(closeAdded(p.vol(), 1. / 6));
    });
    return finishCases(10);
}
