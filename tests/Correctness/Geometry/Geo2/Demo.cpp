#include "../../../../src/Geometry/Geo2/PolygonConvex.hpp"
#include "../../../Support/TestSupport.hpp"

int main() {
    using I = long long;
    std::istringstream input("0 0 4 0 4 3 0 3 2 1 0 0");
    std::vector<Point<I>> ps(6);
    for (auto &p : ps)
        input >> p;
    auto hull = convexHull(std::move(ps));
    CHECK(hull.size() == 4);
    CHECK(hull.area2() == 24);
    CHECK(hull.loc({1, 1}) == Location::IN);
    CHECK(hull.loc({4, 1}) == Location::ON);
    CHECK(hull.loc({5, 1}) == Location::OUT);
    CHECK(inter(hull, Seg<I>{{-1, 1}, {5, 1}}, nullptr));
    return 0;
}
