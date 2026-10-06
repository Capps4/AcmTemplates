#include "../../../../src/Geometry/Geo2/PolygonConvex.hpp"
#include "../../../Support/CaseSupport.hpp"
bool closeAdded(double a, double b) {
    return std::abs(a - b) <= 1e-8 * (1 + std::abs(a) + std::abs(b));
}
using P = Point<double>;
using V = Vec<double>;

int main() {
    runCase("Geo2/PolygonConvex/01-empty-hull", [] {
        CHECK(convexHull(std::vector<P>{}).empty());
    });
    runCase("Geo2/PolygonConvex/02-single-hull", [] {
        auto h = convexHull(std::vector<P>{{2, 3}, {2, 3}});
        CHECK(h.size() == 1);
        CHECK(h.loc(P(2, 3)) == Location::ON);
    });
    runCase("Geo2/PolygonConvex/03-collinear-hull", [] {
        auto h = convexHull(std::vector<P>{{2, 0}, {0, 0}, {1, 0}, {3, 0}, {2, 0}});
        CHECK(h.size() == 2);
        CHECK(h.loc(P(1, 0)) == Location::ON);
    });
    runCase("Geo2/PolygonConvex/04-square-area", [] {
        Polygon<double> p{{{0, 0}, {4, 0}, {4, 3}, {0, 3}}};
        CHECK(p.area() == 12);
        CHECK(p.perimeter() == 14);
        CHECK(p.centroid() == P(2, 1.5));
    });
    runCase("Geo2/PolygonConvex/05-clockwise", [] {
        Polygon<double> p{{{0, 0}, {0, 3}, {4, 3}, {4, 0}}};
        CHECK(p.area() == 12);
        CHECK(p.loc(P(2, 1)) == Location::IN);
        CHECK(p.loc(P(4, 1)) == Location::ON);
    });
    runCase("Geo2/PolygonConvex/06-concave-notch", [] {
        Polygon<double> p{{{0, 0}, {4, 0}, {4, 4}, {2, 2}, {0, 4}}};
        CHECK(!p.isConvex());
        CHECK(p.loc(P(2, 3)) == Location::OUT);
        CHECK(p.loc(P(2, 1)) == Location::IN);
    });
    runCase("Geo2/PolygonConvex/07-interior-points", [] {
        auto h = convexHull(std::vector<P>{{0, 0}, {4, 0}, {4, 3}, {0, 3}, {2, 1}, {2, 2}, {0, 0}});
        CHECK(h.size() == 4);
        CHECK(h.area() == 12);
    });
    runCase("Geo2/PolygonConvex/08-cut-square", [] {
        auto h = convexHull(std::vector<P>{{0, 0}, {4, 0}, {4, 4}, {0, 4}});
        auto cut = cutLeft(h, Line<double>{{2, -1}, {2, 5}});
        CHECK(closeAdded(cut.area(), 8));
    });
    runCase("Geo2/PolygonConvex/09-minkowski", [] {
        auto a = convexHull(std::vector<P>{{0, 0}, {1, 0}, {1, 1}, {0, 1}});
        auto c = minkowskiSum(a, a);
        CHECK(c.area() == 4);
    });
    runCase("Geo2/PolygonConvex/10-diameter", [] {
        auto h = convexHull(std::vector<P>{{0, 0}, {4, 0}, {4, 3}, {0, 3}});
        CHECK(closeAdded(diameter(h), 5));
        CHECK(closeAdded(minWidth(h), 3));
    });
    return finishCases(10);
}
