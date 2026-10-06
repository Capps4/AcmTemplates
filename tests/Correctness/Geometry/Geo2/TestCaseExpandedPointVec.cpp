#include "../../../../src/Geometry/Geo2/PointVec.hpp"
#include "../../../Support/CaseSupport.hpp"
bool closeAdded(double a, double b) {
    return std::abs(a - b) <= 1e-8 * (1 + std::abs(a) + std::abs(b));
}
using P = Point<double>;
using V = Vec<double>;

int main() {
    runCase("Geo2/PointVec/01-origin", [] {
        CHECK(P::O == P(0, 0));
        CHECK(V::O == V(0, 0));
    });
    runCase("Geo2/PointVec/02-point-vector-types", [] {
        static_assert(!std::is_same_v<P, V>);
        P p(1, 2);
        V v(3, -5);
        CHECK(p + v == P(4, -3));
        CHECK((p + v) - p == v);
    });
    runCase("Geo2/PointVec/03-negative-dot", [] {
        V a(-3, 4), b(7, -2);
        CHECK(a.dot(b) == -29);
        CHECK(a.cross(b) == -22);
    });
    runCase("Geo2/PointVec/04-integer-orientation", [] {
        CHECK(orient(Point<long long>(-1000000000, 0), Point<long long>(1000000000, 0),
                     Point<long long>(0, 1000000000)) == 1);
    });
    runCase("Geo2/PointVec/05-distance", [] {
        P a(-1, -2), b(2, 2);
        CHECK(a.dist2(b) == 25);
        CHECK(closeAdded(a.dist(b), 5));
    });
    runCase("Geo2/PointVec/06-rotate-90", [] {
        V a(3, 4);
        CHECK(a.rot90() == V(-4, 3));
        CHECK(a.dot(a.rot90()) == 0);
    });
    runCase("Geo2/PointVec/07-rotate-angle", [] {
        auto v = V(2, 0).rot(std::acos(-1.) / 2);
        CHECK(closeAdded(v.x, 0));
        CHECK(closeAdded(v.y, 2));
    });
    runCase("Geo2/PointVec/08-signed-angle", [] {
        CHECK(closeAdded(V(1, 0).angle(V(0, -1)), -std::acos(-1.) / 2));
    });
    runCase("Geo2/PointVec/09-polar-half", [] {
        CHECK(V(1, 0).half() == 0);
        CHECK(V(-1, 0).half() == 1);
        CHECK(V(0, 1).half() == 0);
        CHECK(V(0, -1).half() == 1);
    });
    runCase("Geo2/PointVec/10-point-hit", [] {
        CHECK(inter(P(1, 2), P(1, 2)).kind == HitKind::ONE);
        CHECK(!inter(P(1, 2), P(2, 1)));
        CHECK(near(P(1, 2), P(2, 1)).first == P(1, 2));
    });
    return finishCases(10);
}
