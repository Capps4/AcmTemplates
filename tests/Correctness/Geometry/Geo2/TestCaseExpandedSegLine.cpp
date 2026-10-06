#include "../../../../src/Geometry/Geo2/SegLine.hpp"
#include "../../../Support/CaseSupport.hpp"
bool closeAdded(double a, double b) {
    return std::abs(a - b) <= 1e-8 * (1 + std::abs(a) + std::abs(b));
}
using P = Point<double>;
using V = Vec<double>;

int main() {
    runCase("Geo2/SegLine/01-proper-cross", [] {
        auto h = inter(Seg<double>{{0, 0}, {4, 4}}, Seg<double>{{0, 4}, {4, 0}});
        CHECK(h.kind == HitKind::ONE);
        CHECK(h.ps[0] == P(2, 2));
    });
    runCase("Geo2/SegLine/02-endpoint-contact", [] {
        CHECK(inter(Seg<double>{{0, 0}, {1, 0}}, Seg<double>{{1, 0}, {1, 2}}).kind == HitKind::ONE);
    });
    runCase("Geo2/SegLine/03-overlap", [] {
        auto h = inter(Seg<double>{{0, 0}, {4, 0}}, Seg<double>{{1, 0}, {3, 0}});
        CHECK(h.kind == HitKind::SEG);
        CHECK(h.ps[0].dist2(h.ps[1]) == 4);
    });
    runCase("Geo2/SegLine/04-parallel-disjoint", [] {
        CHECK(!inter(Line<double>{{0, 0}, {1, 0}}, Line<double>{{0, 1}, {1, 1}}));
    });
    runCase("Geo2/SegLine/05-coincident-lines", [] {
        CHECK(inter(Line<double>{{0, 0}, {1, 0}}, Line<double>{{2, 0}, {-3, 0}}).kind ==
              HitKind::CO);
    });
    runCase("Geo2/SegLine/06-zero-segment", [] {
        Seg<double> s{{2, 3}, {2, 3}};
        CHECK(s.loc(P(2, 3)) == Location::ON);
        CHECK(near(P(7, 3), s).second == P(2, 3));
    });
    runCase("Geo2/SegLine/07-outside-nearest", [] {
        CHECK(near(P(-1, 2), Seg<double>{{0, 0}, {4, 0}}).second == P(0, 0));
        CHECK(near(P(5, 2), Seg<double>{{0, 0}, {4, 0}}).second == P(4, 0));
    });
    runCase("Geo2/SegLine/08-projection", [] {
        Line<double> l{{0, 0}, {4, 0}};
        CHECK(near(P(2, 3), l).second == P(2, 0));
        CHECK(l.reflect(P(2, 3)) == P(2, -3));
    });
    runCase("Geo2/SegLine/09-reversed-line", [] {
        Line<double> l{{0, 0}, {4, 0}};
        CHECK(l.side(P(2, 3)) == -l.reversed().side(P(2, 3)));
    });
    runCase("Geo2/SegLine/10-integer-boundary", [] {
        Seg<long long> s{{-1000000000, -1000000000}, {1000000000, 1000000000}};
        CHECK(s.loc(Point<long long>(0, 0)) == Location::ON);
        CHECK(s.loc(Point<long long>(0, 1)) == Location::OUT);
    });
    return finishCases(10);
}
