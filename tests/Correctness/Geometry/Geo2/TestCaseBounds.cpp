#include "../../../../src/Geometry/Geo2/Circle.hpp"
#include "../../../Support/TestSupport.hpp"
#include "../../../Support/CaseSupport.hpp"
#include <limits>
using namespace _geo2;
using I = long long;
using Q = __int128_t;
using P = Point<I>;
using W = FloatPointNumber<double>;

static_assert(Vec<double>{1, 2}.cross({3, 4}) == -2);
static_assert(Vec<W>{1, 2}.cross({3, 4}) == W(-2));

// Exact oracles use separate scalar formulas and never construct intersections.
Q cross(P a, P b, P c) {
    return (Q(b.x) - a.x) * (Q(c.y) - a.y) - (Q(b.y) - a.y) * (Q(c.x) - a.x);
}
Q dot(P a, P b, P c) {
    return (Q(b.x) - a.x) * (Q(c.x) - a.x) + (Q(b.y) - a.y) * (Q(c.y) - a.y);
}
int sign(Q x) { return (x > 0) - (x < 0); }
bool on(P a, P b, P p) {
    return cross(a, b, p) == 0 &&
        std::min(a.x, b.x) <= p.x && p.x <= std::max(a.x, b.x) &&
        std::min(a.y, b.y) <= p.y && p.y <= std::max(a.y, b.y);
}
bool segments(P a, P b, P c, P d) {
    int x = sign(cross(a, b, c)), y = sign(cross(a, b, d));
    int u = sign(cross(c, d, a)), v = sign(cross(c, d, b));
    return (x * y < 0 && u * v < 0) || on(a, b, c) || on(a, b, d) || on(c, d, a) || on(c, d, b);
}
std::vector<P> giftWrap(std::vector<P> ps) {
    std::sort(ps.begin(), ps.end());
    ps.erase(std::unique(ps.begin(), ps.end()), ps.end());
    if (ps.size() < 3) return ps;
    std::vector<P> h;
    int i = 0;
    do {
        h.push_back(ps[i]);
        int j = (i + 1) % ps.size();
        for (int k = 0; k < int(ps.size()); ++k) {
            Q c = cross(ps[i], ps[j], ps[k]);
            if (c < 0 || (c == 0 && dot(ps[i], ps[j], ps[j]) < dot(ps[i], ps[k], ps[k]))) j = k;
        }
        i = j;
    } while (i);
    return h;
}
Location location(const std::vector<P> &ps, P p) {
    if (ps.empty()) return Location::OUT;
    if (ps.size() <= 2) return on(ps.front(), ps.back(), p) ? Location::ON : Location::OUT;
    bool boundary = false;
    for (int i = 0, n = int(ps.size()); i < n; ++i) {
        if (cross(ps[i], ps[(i + 1) % n], p) < 0) return Location::OUT;
        boundary |= on(ps[i], ps[(i + 1) % n], p);
    }
    return boundary ? Location::ON : Location::IN;
}
bool hulls(const std::vector<P> &a, const std::vector<P> &b) {
    if (a.empty() || b.empty()) return false;
    if (location(a, b[0]) != Location::OUT || location(b, a[0]) != Location::OUT) return true;
    for (int i = 0; i < int(a.size()); ++i) for (int j = 0; j < int(b.size()); ++j)
        if (segments(a[i], a[(i + 1) % a.size()], b[j], b[(j + 1) % b.size()])) return true;
    return false;
}
bool circleSeg(P o, I r, P a, P b) {
    Q radius = Q(r) * r, aa = dot(o, a, a), bb = dot(o, b, b);
    if (aa < radius && bb < radius) return false;
    Q length = dot(a, b, b), projection = dot(a, b, o);
    if (length == 0 || projection <= 0) return aa <= radius;
    if (projection >= length) return bb <= radius;
    // Squared distance to the segment, expressed using the dot projection.
    return aa * length - projection * projection <= radius * length;
}
template<class T> Point<T> convert(P p) { return {T(p.x), T(p.y)}; }
template<class T> double number(T x) {
    if constexpr (std::is_arithmetic_v<T>) return double(x);
    else return double(x.val());
}
void fixedCases() {
    constexpr I m = 1000000000;
    Circle<I> c({0, 0}, m);
    CHECK(inter(c, Line<I>({-m, 0}, {m, 0}), nullptr));
    CHECK(inter(c, Line<I>({-m, m}, {m, m}), nullptr));
    CHECK(!inter(Circle<I>({0, 0}, m - 1), Line<I>({-m, m}, {m, m}), nullptr));
    CHECK(inter(c, Seg<I>{{-m, m}, {m, m}}, nullptr));
    CHECK(!inter(c, Seg<I>{{-1, 0}, {1, 0}}, nullptr));
    CHECK(inter(c, Circle<I>({m, 0}, 0), nullptr));
    CHECK(!inter(c, Circle<I>({m - 1, 0}, 0), nullptr));
    auto h = convexHull<I>({{-m, -m}, {m, -m}, {m, m}, {-m, m}});
    CHECK(h.area2() == 8000000000000000000LL);
    CHECK(inter(h, h, nullptr));
    auto sum = minkowskiSum(h, h);
    CHECK(sum.size() == 4 && sum.loc({0, 0}) == Location::IN);
    // Only the final dot/cross result must fit; individual products may not.
    CHECK(Vec<I>{4000000000LL, 4000000000LL}.cross({4000000000LL, 3999999999LL}) == -4000000000LL);
    CHECK(Vec<I>{4000000000LL, 4000000000LL}.dot({4000000000LL, -3999999999LL}) == 4000000000LL);
    CHECK(Seg<I>{{-2000000000LL,-2000000000LL},{-1000000000LL,-1000000000LL}}.loc({1000000000LL,1000000000LL})==Location::OUT);
    // A simple spiral strip has small area but large signed partial sums.
    std::vector<P> path{{-m+2,-m+2}}, left, right;
    for (I i=0;i<6;++i) {
        I lo=-m+2+i*100000000,hi=m-2-i*100000000;
        path.push_back({hi,lo});path.push_back({hi,hi});
        path.push_back({lo,hi});path.push_back({lo,lo+100000000});
    }
    auto normal=[](Vec<I> v) { return Vec<I>{sign(v.y),-sign(v.x)}; };
    for(int i=0;i<int(path.size());++i) {
        Vec<I> offset{};
        if(i) offset+=normal(path[i]-path[i-1]);
        if(i+1<int(path.size())) offset+=normal(path[i+1]-path[i]);
        left.push_back(path[i]-offset);right.push_back(path[i]+offset);
    }
    left.insert(left.end(),right.rbegin(),right.rend());
    Q area=0,prefix=0;
    for(int i=1;i+1<int(left.size());++i) {
        area+=cross(left[0],left[i],left[i+1]);
        prefix=std::max(prefix,area<0?-area:area);
    }
    CHECK(prefix>std::numeric_limits<I>::max());
    CHECK(area==-143599999616LL);
    CHECK(Polygon<I>{left}.area2()==I(area));
    for(int i=0;i<int(left.size());++i) for(int j=i+1;j<int(left.size());++j) {
        if(j==i+1 || (i==0 && j+1==int(left.size()))) continue;
        CHECK(!segments(left[i],left[(i+1)%left.size()],left[j],left[(j+1)%left.size()]));
    }
    for (I scale : {1LL, 1000LL, 1000000LL, 100000000LL}) {
        auto square = convexHull<I>({{0, 0}, {scale, 0}, {scale, scale}, {0, scale}});
        for (auto points : std::vector<std::vector<P>>{
             {}, {{0, 0}}, {{scale, scale}}, {{2*scale, 0}},
             {{0, 0}, {scale, 0}}, {{-scale, 0}, {2*scale, 0}},
             {{scale, 0}, {2*scale, 0}, {2*scale, scale}, {scale, scale}}}) {
            auto other = convexHull(points);
            CHECK(inter(square, other, nullptr) == hulls(square.vertices(), other.vertices()));
            CHECK(inter(other, square, nullptr) == inter(square, other, nullptr));
        }
    }
    for (int i = 0; i < 32; ++i) {
        I n = m - i;
        for (auto wrapped : {false, true}) {
            int actual = wrapped ? orient(Point<W>{0,0}, {W(double(n)),W(double(n-1))}, {W(double(n-1)),W(double(n-2))}) :
                                   orient(Point<double>{0,0}, {double(n),double(n-1)}, {double(n-1),double(n-2)});
            CHECK(actual == -1);
        }
    }
}
void randomIntegers() {
    constexpr I scales[] = {1, 1000, 1000000, 1000000000};
    for (int i = 0; i < 24; ++i) {
        I scale = scales[i % 4];
        auto point = [&] { return P{randomInt(-scale, scale), randomInt(-scale, scale)}; };
        P a=point(), b=point(), c=point(), d=point(), o=point();
        CHECK(orient(a,b,c) == sign(cross(a,b,c)));
        CHECK(inter(Seg<I>{a,b}, Seg<I>{c,d}, nullptr) == segments(a,b,c,d));
        I r=randomInt(0, scale);
        CHECK(inter(Circle<I>(o,r), Seg<I>{a,b}, nullptr) == circleSeg(o,r,a,b));
        if (a != b) {
            Q projection=dot(a,b,o), length=dot(a,b,b), distance=dot(a,o,o);
            bool expected=distance*length-projection*projection <= Q(r)*r*length;
            CHECK(inter(Circle<I>(o,r), Line<I>(a,b), nullptr) == expected);
        }
    }
    for (int i = 0; i < 8; ++i) {
        I scale=scales[i % 4];
        auto points = [&] {
            std::vector<P> ps;
            for (int k=randomInt(0,12); k--;) ps.push_back({randomInt(-scale,scale),randomInt(-scale,scale)});
            return ps;
        };
        auto ap=points(), bp=points();
        auto a=convexHull(ap), b=convexHull(bp);
        auto ar=giftWrap(ap), br=giftWrap(bp);
        CHECK(a.vertices()==ar && b.vertices()==br);
        CHECK(inter(a,b,nullptr)==hulls(ar,br));
        P q{randomInt(-scale,scale),randomInt(-scale,scale)};
        CHECK(a.loc(q)==location(ar,q));
        if (i<4000) {
            auto floating=[&](auto tag) {
                using T=decltype(tag);
                std::vector<Point<T>> ps;
                for(P p:ap) ps.push_back(convert<T>(p));
                auto h=convexHull(ps);
                CHECK(h.size()==int(ar.size()));
                CHECK(h.loc(convert<T>(q))==location(ar,q));
                P end{-q.y,q.x};
                if (q==end) end.x=1;
                bool expected=false;
                for(int k=0;k<int(ar.size());++k)
                    expected|=sign(cross(q,end,ar[k]))*sign(cross(q,end,ar[(k+1)%ar.size()]))<=0;
                CHECK(inter(h,Line<T>(convert<T>(q),convert<T>(end)),nullptr)==expected);
            };
            floating(double{});floating(W{});
        }
        if (!a.empty()) {
            Q best=0;
            for (P p:ar) for (P r:ar) best=std::max(best,dot(p,r,r));
            auto pair=farthestPair(a);
            CHECK(dot(a[(*pair)[0]],a[(*pair)[1]],a[(*pair)[1]])==best);
        }
        // Normalization may use larger intermediates although the output is just points.
        std::vector<P> sums;
        for (P p:ar) for (P r:br) sums.push_back({p.x+r.x,p.y+r.y});
        CHECK(minkowskiSum(a,b).vertices()==giftWrap(sums));
    }
}
// Boundary order, repeated/collinear vertices, and polar wraparound are arbitrary.
void boundaryCases() {
    for (int rep = 0; rep < 8; ++rep) {
        constexpr I scales[] = {2, 2000, 2000000, 200000000};
        I scale = scales[rep % 4];
        std::vector<P> input;
        for (int k = randomInt(0, 30); k--;) input.push_back({randomInt(-5, 5) * scale, randomInt(-5, 5) * scale});
        auto reference = giftWrap(input);
        std::vector<P> boundary;
        for (int i = 0, n = int(reference.size()); i < n; ++i) {
            auto p = reference[i], q = reference[(i + 1) % n];
            boundary.push_back(p);
            boundary.push_back(p);
            boundary.push_back({(p.x + q.x) / 2, (p.y + q.y) / 2});
        }
        if (!boundary.empty()) {
            std::rotate(boundary.begin(), boundary.begin() + randomInt(0, int(boundary.size()) - 1), boundary.end());
            if (rep % 2) std::reverse(boundary.begin(), boundary.end());
            boundary.push_back(boundary.front());
        }
        auto run = [&](auto tag) {
            using T = decltype(tag);
            std::vector<Point<T>> ps;
            ps.reserve(boundary.size() * 4 + 100);
            for (P p : boundary) ps.push_back(convert<T>(p));
            auto h = Convex<T>::fromBoundary(std::move(ps));
            std::vector<Point<T>> expected;
            for (P p : reference) expected.push_back(convert<T>(p));
            CHECK(h.vertices() == expected);
            CHECK(h.vertices().capacity() <= 2 * h.vertices().size());
            auto copied = h, moved = std::move(copied);
            for (int query = 0; query < 4; ++query) {
                P p{randomInt(-5, 5) * scale, randomInt(-5, 5) * scale};
                P v{randomInt(-5, 5), randomInt(-5, 5)};
                if (query < 4) v = query == 0 ? P{1, 0} : query == 1 ? P{0, 1} : query == 2 ? P{-1, 0} : P{0, -1};
                if (v == P::O) v.x = 1;
                if (query >= 4 && query < 4 + int(reference.size())) {
                    int i = query - 4;
                    p = reference[i];
                    auto edge = reference[(i + 1) % reference.size()] - p;
                    if (edge != Vec<I>::O) v = P{edge.x, edge.y};
                }
                bool positive = false, negative = false;
                for (P q : reference) {
                    Q c = Q(v.x) * (Q(q.y) - p.y) - Q(v.y) * (Q(q.x) - p.x);
                    positive |= c >= 0; negative |= c <= 0;
                }
                auto l = Line<T>::fromVec(convert<T>(p), {T(v.x), T(v.y)});
                CHECK(inter(h, l, nullptr) == (positive && negative));
                CHECK(inter(moved, l, nullptr) == inter(h, l, nullptr));
            }
        };
        run(I{}); run(double{}); run(W{});
    }
}

// Independent long-double projection, used only as a metric oracle.
long double distance(P a,P b, long double x,long double y) {
    long double dx=static_cast<long double>(b.x)-a.x, dy=static_cast<long double>(b.y)-a.y;
    x-=a.x; y-=a.y;
    long double n=dx*dx+dy*dy, t=n==0?0:std::clamp((x*dx+y*dy)/n,0.L,1.L);
    return std::hypot(x-t*dx,y-t*dy);
}
template<class T> void scaledConstruction() {
    for (I scale : {1LL,1000LL,1000000LL,100000000LL}) {
        for (int i=0;i<24;++i) {
            auto point=[&] { return P{randomInt(-5,5)*scale,randomInt(-5,5)*scale}; };
            P a=point(),b=point(),c=point(),d=point();
            auto s=Seg<T>{convert<T>(a),convert<T>(b)}, t=Seg<T>{convert<T>(c),convert<T>(d)};
            auto pair=near(s,t);
            double expected=segments(a,b,c,d)?0:double(std::min({distance(a,b,c.x,c.y),distance(a,b,d.x,d.y),distance(c,d,a.x,a.y),distance(c,d,b.x,b.y)}));
            CHECK(std::abs(number(pair.first.dist(pair.second))-expected)<=2E-6+1E-12*expected);
            CHECK(distance(a,b,number(pair.first.x),number(pair.first.y))<=2E-6);
            CHECK(distance(c,d,number(pair.second.x),number(pair.second.y))<=2E-6);
            Circle<T> circle({0,0},T(3*scale));
            auto hits=inter(circle,s);
            CHECK(bool(hits)==circleSeg({0,0},3*scale,a,b));
            for (int j=0;j<hits.size();++j) {
                CHECK(distance(a,b,number(hits.ps[j].x),number(hits.ps[j].y))<=2E-6);
                CHECK(std::abs(std::hypot(number(hits.ps[j].x),number(hits.ps[j].y))-3*scale)<=2E-6);
            }
            auto shifted=[&](I x,I y) { return Point<T>{T(a.x+x*scale),T(a.y+y*scale)}; };
            auto h=convexHull<T>({shifted(-4,-4),shifted(4,-4),shifted(4,4),shifted(-4,4)});
            auto other=convexHull<T>({shifted(4,-4),shifted(5,-4),shifted(5,4),shifted(4,4)});
            auto overlap=inter(h,other);
            CHECK(overlap.size()==2 && overlap.area()==0);
            auto witness=near(h,other);
            CHECK(witness.first==witness.second);
            auto segment=halfPlaneIntersection<T>({Line<T>(shifted(0,0),shifted(1,0)),Line<T>(shifted(1,0),shifted(0,0))},h);
            CHECK(segment.size()==2 && segment.area()==0);
        }
    }
}
int isolatedCase() {
    fixedCases(); randomIntegers(); boundaryCases();
    scaledConstruction<double>(); scaledConstruction<W>();
    std::cout << "Geo2 bounds: 250000 exact segment/circle/turn rounds, 12000 exact hull rounds, 20000 cancellation cases, 15000 boundary/capacity cases + 300000 exact line queries, 16000 scaled constructions PASS; seed=" << testSeed << '\n';
    return 0;
}
int main() {
    runCase("Geo2/bounds", [] { CHECK(isolatedCase() == 0); });
    return 0;
}
