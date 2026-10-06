#include "../../../../src/Geometry/Geo2/Circle.hpp"
#include "../../../Support/CaseSupport.hpp"
bool closeAdded(double a, double b) {
    return std::abs(a - b) <= 1e-8 * (1 + std::abs(a) + std::abs(b));
}
using P = Point<double>;
using V = Vec<double>;

int main() {
    runCase("Geo2/Circle/01-zero-circle", [] {
        Circle<double> c{{1, 2}, 0};
        CHECK(c.loc(P(1, 2)) == Location::ON);
        CHECK(inter(c, c).kind == HitKind::ONE);
    });
    runCase("Geo2/Circle/02-point-location", [] {
        Circle<double> c{{0, 0}, 5};
        CHECK(c.loc(P(0, 0)) == Location::IN);
        CHECK(c.loc(P(3, 4)) == Location::ON);
        CHECK(c.loc(P(6, 0)) == Location::OUT);
    });
    runCase("Geo2/Circle/03-line-tangent", [] {
        auto h = inter(Circle<double>{{0, 0}, 2}, Line<double>{{-3, 2}, {3, 2}});
        CHECK(h.kind == HitKind::ONE);
        CHECK(h.ps[0] == P(0, 2));
    });
    runCase("Geo2/Circle/04-line-secant", [] {
        auto h = inter(Circle<double>{{0, 0}, 2}, Line<double>{{-3, 0}, {3, 0}});
        CHECK(h.kind == HitKind::TWO);
        for (int i = 0; i < 2; ++i)
            CHECK(h.ps[i].dist2(P(0, 0)) == 4);
    });
    runCase("Geo2/Circle/05-external-tangent", [] {
        Circle<double> a{{0, 0}, 2}, b{{4, 0}, 2};
        CHECK(relation(a, b) == CircleRelation::EXTERNAL_TANGENT);
        CHECK(inter(a, b).ps[0] == P(2, 0));
    });
    runCase("Geo2/Circle/06-internal-tangent", [] {
        Circle<double> a{{0, 0}, 3}, b{{2, 0}, 1};
        CHECK(relation(a, b) == CircleRelation::INTERNAL_TANGENT);
        CHECK(inter(a, b).ps[0] == P(3, 0));
    });
    runCase("Geo2/Circle/07-contained", [] {
        Circle<double> a{{0, 0}, 3}, b{{0, 0}, 1};
        CHECK(relation(a, b) == CircleRelation::CONTAINED);
        CHECK(!inter(a, b));
        CHECK(closeAdded(intersectionArea(a, b), std::acos(-1.)));
    });
    runCase("Geo2/Circle/08-circumcircle", [] {
        auto c = Circle<double>::circum(P(0, 0), P(2, 0), P(0, 2));
        CHECK(c.o == P(1, 1));
        CHECK(closeAdded(c.r, std::sqrt(2.)));
    });
    runCase("Geo2/Circle/09-segment-rim", [] {
        Circle<double> c{{0, 0}, 1};
        CHECK(!inter(c, Seg<double>{{0, 0}, {0.5, 0}}));
        CHECK(inter(c, Seg<double>{{0, 0}, {2, 0}}).kind == HitKind::ONE);
    });
    runCase("Geo2/Circle/10-point-tangents", [] {
        Circle<double> c{{0, 0}, 1};
        auto t = tangents(P(2, 0), c);
        CHECK(t.lines.size() == 2);
        for (auto &l : t.lines)
            CHECK(closeAdded(near(c.o, l).first.dist(near(c.o, l).second), 1));
    });
    return finishCases(10);
}
