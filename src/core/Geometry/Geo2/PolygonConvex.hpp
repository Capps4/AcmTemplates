#pragma once
#include "SegLine.hpp"

// SNIPPET BEGIN
namespace _geo2 {

template <class T>
class Convex;
template <class T>
Convex<T> convexHull(std::vector<Point<T>> ps);

template <class T>
struct Polygon {
    std::vector<Point<T>> ps{};
    int size() const {
        return int(ps.size());
    }
    bool empty() const {
        return ps.empty();
    }
    Point<T> &operator[](int i) {
        return ps[i];
    }
    const Point<T> &operator[](int i) const {
        return ps[i];
    }
    Point<T> &front() {
        return ps.front();
    }
    const Point<T> &front() const {
        return ps.front();
    }
    Point<T> &back() {
        return ps.back();
    }
    const Point<T> &back() const {
        return ps.back();
    }
    auto begin() {
        return ps.begin();
    }
    auto end() {
        return ps.end();
    }
    auto begin() const {
        return ps.begin();
    }
    auto end() const {
        return ps.end();
    }
    auto cbegin() const {
        return ps.cbegin();
    }
    auto cend() const {
        return ps.cend();
    }
    T area2() const {
        using R = std::conditional_t<std::is_integral_v<T>, __int128_t, T>;
        R ans = 0;
        for (int i = 1; i + 1 < size(); ++i)
            ans += ps[0].cross(ps[i], ps[i + 1]);
        return T(ans);
    }
    auto area() const {
        using R = decltype(Vec<T>{}.len());
        return R(std::abs(area2())) / R(2);
    }
    auto perimeter() const {
        using R = decltype(Point<T>{}.dist(Point<T>{}));
        R ans = 0;
        for (int i = 0; i < size(); ++i)
            ans += edge(i).len();
        return ans;
    }
    Location loc(Point<T> p) const {
        int wn = 0;
        for (int i = 0; i < size(); ++i) {
            auto s = edge(i);
            if (s.loc(p) == Location::ON)
                return Location::ON;
            int c = orient(s.a, s.b, p);
            if (s.a.y <= p.y and p.y < s.b.y and c > 0)
                ++wn;
            if (s.b.y <= p.y and p.y < s.a.y and c < 0)
                --wn;
        }
        return wn ? Location::IN : Location::OUT;
    }
    // The boundary must be simple. Collinear turns are allowed.
    bool isConvex() const {
        int turn = 0;
        for (int i = 0; i < size(); ++i) {
            int c = orient(ps[i], ps[(i + 1) % size()], ps[(i + 2) % size()]);
            if (c and turn and c != turn)
                return false;
            if (c)
                turn = c;
        }
        return true;
    }
    Point<T> centroid() const {
        static_assert(!std::is_integral_v<T>, "centroid needs floating coordinates");
        T a = area2();
        assert(a != 0);
        Vec<T> sum{};
        for (int i = 0; i < size(); ++i) {
            auto p = ps[i], q = edge(i).b;
            sum += (p.toVec() + q.toVec()) * p.toVec().cross(q.toVec());
        }
        return Point<T>::O + sum / (a * T(3));
    }
    Convex<T> hull() const {
        return convexHull(ps);
    }

private:
    Seg<T> edge(int i) const {
        return {ps[i], ps[(i + 1) % size()]};
    }
    template <class U>
    friend std::vector<Seg<U>> inter(const Polygon<U> &, const Line<U> &);
};

template <class T>
struct BoundingRect;

template <class T>
class Convex {
    enum class LineHullKind { NONE, VERTEX, EDGE, CROSS };
    struct LineHullHit {
        LineHullKind kind = LineHullKind::NONE;
        int a = -1, b = -1;
    };
    template <class U>
    friend Hit<U> inter(const Convex<U> &, const Line<U> &);
    template <class U>
    friend bool inter(const Convex<U> &, const Line<U> &, std::nullptr_t);
    template <class U>
    friend bool inter(const Convex<U> &, const Seg<U> &, std::nullptr_t);
    template <class U>
    friend bool inter(const Convex<U> &, const Convex<U> &, std::nullptr_t);
    template <class U>
    friend Convex<U> cutLeft(const Convex<U> &, const Line<U> &);
    template <class U>
    friend Near<U> near(Point<U>, const Convex<U> &);
    template <class U>
    friend Near<U> near(const Convex<U> &, const Line<U> &);
    template <class U>
    friend Near<U> near(const Convex<U> &, const Convex<U> &);
    template <class U>
    friend std::optional<std::array<int, 2>> farthestPair(const Convex<U> &);
    template <class U>
    friend auto minWidth(const Convex<U> &);
    template <class U>
    friend std::optional<BoundingRect<U>> minBoundingRect(const Convex<U> &);
    template <class U>
    friend Convex<U> minkowskiSum(const Convex<U> &, const Convex<U> &);
    template <class U>
    friend Convex<U> minkowskiDiff(const Convex<U> &, const Convex<U> &);
    template <class U>
    friend Convex<U> halfPlaneIntersection(std::vector<Line<U>>, const Convex<U> &);
    template <class U>
    friend Convex<U> inter(const Convex<U> &, const Convex<U> &);
    std::vector<Point<T>> ps{};
    int low = 0; // Lowest vertex, then leftmost: the first edge in polar order.
public:
    Convex() = default;
    // O(n). Input must already be a convex boundary, in either orientation.
    static Convex fromBoundary(std::vector<Point<T>> p) {
        p.erase(std::unique(p.begin(), p.end()), p.end());
        if (p.size() > 1 and p.front() == p.back())
            p.pop_back();
        if (p.size() > 2) {
            int a = 0;
            for (int i = 1; !a and i + 1 < int(p.size()); ++i)
                a = orient(p[0], p[i], p[i + 1]);
            if (a == 0) {
                auto mm = std::minmax_element(p.begin(), p.end());
                p = {*mm.first, *mm.second};
            } else {
                if (a < 0)
                    std::reverse(p.begin(), p.end());
                // Remove collinear vertices incrementally, so near-duplicate
                // intersections cannot erase both representatives of a corner.
                int n = 0;
                for (auto v : p) {
                    while (n > 1 and !orient(p[n - 2], p[n - 1], v))
                        --n;
                    p[n++] = v;
                }
                while (n > 2 and !orient(p[n - 2], p[n - 1], p[0]))
                    --n;
                int first = 0;
                while (n - first > 2 and !orient(p[n - 1], p[first], p[first + 1]))
                    ++first;
                p.resize(n);
                p.erase(p.begin(), p.begin() + first);
            }
        }
        if (!p.empty())
            std::rotate(p.begin(), std::min_element(p.begin(), p.end()), p.end());
        // Release oversized work buffers without reallocating ordinary boundaries.
        if (p.capacity() > 2 * p.size())
            std::vector<Point<T>>(p).swap(p);
        Convex h;
        h.ps = std::move(p);
        // From the leftmost vertex, downward/rightward edges form one prefix.
        int l = 0, r = h.size();
        while (l < r) {
            int m = (l + r) / 2;
            const auto &a = h[m], &b = h.at(m + 1);
            if (b.x > a.x and b.y < a.y)
                l = m + 1;
            else
                r = m;
        }
        h.low = l;
        return h;
    }
    int size() const {
        return int(ps.size());
    }
    bool empty() const {
        return ps.empty();
    }
    const std::vector<Point<T>> &vertices() const {
        return ps;
    }
    const Point<T> &operator[](int i) const {
        return ps[i];
    }
    const Point<T> &front() const {
        return ps.front();
    }
    const Point<T> &back() const {
        return ps.back();
    }
    // Read-only access preserves the canonical boundary and cached indices.
    auto begin() const {
        return ps.begin();
    }
    auto end() const {
        return ps.end();
    }
    auto cbegin() const {
        return ps.cbegin();
    }
    auto cend() const {
        return ps.cend();
    }

private:
    // Internal callers only pass a vertex index or the one-past-end index.
    const Point<T> &at(int i) const {
        return ps[i == size() ? 0 : i];
    }
    Seg<T> edge(int i) const {
        return {ps[i], at(i + 1)};
    }

public:
    Polygon<T> polygon() const {
        return {ps};
    }
    T area2() const {
        using R = std::conditional_t<std::is_integral_v<T>, __int128_t, T>;
        R ans = 0;
        for (int i = 1; i + 1 < size(); ++i)
            ans += ps[0].cross(ps[i], ps[i + 1]);
        return T(ans);
    }
    auto area() const {
        using R = decltype(Vec<T>{}.len());
        return R(std::abs(area2())) / R(2);
    }
    auto perimeter() const {
        using R = decltype(Vec<T>{}.len());
        R ans = 0;
        for (int i = 0; i < size(); ++i)
            ans += edge(i).len();
        return ans;
    }
    Location loc(Point<T> p) const {
        int n = size();
        if (n < 3)
            return n and (Seg<T>{ps[0], ps[n - 1]}.loc(p) == Location::ON) ? Location::ON
                                                                           : Location::OUT;
        int a = orient(ps[0], ps[1], p), b = orient(ps[0], ps[n - 1], p);
        if (a < 0 or b > 0)
            return Location::OUT;
        if (!a)
            return (edge(0).loc(p) == Location::ON) ? Location::ON : Location::OUT;
        if (!b)
            return (Seg<T>{ps[0], ps[n - 1]}.loc(p) == Location::ON) ? Location::ON : Location::OUT;
        int l = 1, r = n - 1;
        while (r - l > 1) {
            int m = (l + r) / 2;
            (orient(ps[0], ps[m], p) >= 0 ? l : r) = m;
        }
        int c = orient(ps[l], ps[r], p);
        return c < 0 ? Location::OUT : c == 0 ? Location::ON : Location::IN;
    }

private:
    // O(log n). Any maximizer is allowed; a zero direction returns vertex 0.
    int peak(Vec<T> v) const {
        assert(!empty());
        int n = size();
        if (n <= 2)
            return n == 2 and v.dot(ps[1] - ps[0]) > 0 ? 1 : 0;
        if (v == Vec<T>::O)
            return 0;
        auto aim = v.rot90();
        int l = 0, r = n, ht = aim.half();
        // Select one of the two physically contiguous polar-sorted ranges.
        if (low) {
            if (cmp(aim, ps[1] - ps[0]))
                l = low;
            else
                r = low;
        }
        while (l < r) {
            int m = (l + r) / 2;
            auto e = at(m + 1) - ps[m];
            int he = e.half();
            bool less = he != ht ? he < ht : e.cross(aim) > 0;
            (less ? l : r) = less ? m + 1 : m;
        }
        return l == n ? 0 : l;
    }
    // KACTL/ACTL line-hull search (Boost license; see LICENSE and README).
    // O(log n). Vertex crossings use the outgoing edge; indices follow l.v.
    LineHullHit hit(const Line<T> &l) const {
        int n = size();
        if (!n)
            return {};
        if (n == 1)
            return (l.loc(ps[0]) == Location::ON) ? LineHullHit{LineHullKind::VERTEX, 0, -1}
                                                  : LineHullHit{};
        if (n == 2) {
            int a = l.side(ps[0]), b = l.side(ps[1]);
            if (!a and !b)
                return {LineHullKind::EDGE, 0, 0};
            if (!a or !b)
                return {LineHullKind::VERTEX, !a ? 0 : 1, -1};
            return a * b < 0 ? LineHullHit{LineHullKind::CROSS, 0, 0} : LineHullHit{};
        }
        int a = peak(-l.v.rot90()), b = peak(l.v.rot90());
        auto side = [&](int i) {
            return -l.side(ps[i]);
        };
        if (side(a) < 0 or side(b) > 0)
            return {LineHullKind::NONE, a, b};
        std::array<int, 2> res;
        for (int k = 0; k < 2; ++k) {
            int lo = b, hi = a;
            while ((lo + 1) % n != hi) {
                int m = ((lo + hi + (lo < hi ? 0 : n)) / 2) % n;
                (side(m) == side(b) ? lo : hi) = m;
            }
            res[k] = (lo + (side(hi) == 0)) % n;
            std::swap(a, b);
        }
        if (res[0] == res[1])
            return {LineHullKind::VERTEX, res[0], -1};
        if (!side(res[0]) and !side(res[1])) {
            if ((res[0] + 1) % n == res[1])
                return {LineHullKind::EDGE, res[0], res[0]};
            if ((res[1] + 1) % n == res[0])
                return {LineHullKind::EDGE, res[1], res[1]};
        }
        return {LineHullKind::CROSS, res[0], res[1]};
    }

public:
    // O(log n), external point only. Returns the two contact vertex indices.
    std::array<int, 2> tangents(Point<T> p) const {
        assert(loc(p) == Location::OUT and !empty());
        if (size() <= 2)
            return {0, size() - 1};
        // Partition by the outgoing edge and the ray from vertex 0.
        // Adapted from the supplied static hull's extreme-direction search.
        auto seek = [&](int sign) {
            auto dir = [&](int i) {
                return (ps[i] - p) * T(sign);
            };
            auto test = [&](int i) {
                T x = dir(i).cross(edge(i).vec());
                return x == 0 or x > 0;
            };
            bool first = test(0);
            if (!first and test(size() - 1))
                return 0;
            auto pred = [&](int i) {
                if (!i)
                    return true;
                bool cur = test(i);
                T x = dir(0).cross(ps[i] - ps[0]);
                if (i == 1 and cur == first and x == 0)
                    return true;
                return bool(cur ^ (cur == first and (x == 0 or x < 0)));
            };
            int l = 0, r = size();
            while (l < r) {
                int m = (l + r) / 2;
                if (pred(m))
                    l = m + 1;
                else
                    r = m;
            }
            return l % size();
        };
        return {seek(1), seek(-1)};
    }

private:
    Point<T> foot(Point<T> p) const {
        assert(!empty());
        if (loc(p) != Location::OUT)
            return p;
        Point<T> ans = ps[0];
        for (int i = 0; i < size(); ++i) {
            auto q = near(p, edge(i)).second;
            if (p.dist2(q) < p.dist2(ans))
                ans = q;
        }
        return ans;
    }
    static bool cmp(Vec<T> a, Vec<T> b) {
        return a.half() != b.half() ? a.half() < b.half() : a.cross(b) > 0;
    }
    std::vector<Line<T>> cuts() const;
    static Convex hpi(const std::vector<Line<T>> &, const Convex &);
    template <class Emit>
    static void walk(const Convex &a, const Convex &b, bool sub, Emit emit);
};

template <class T>
Convex<T> convexHull(std::vector<Point<T>> ps) {
    std::sort(ps.begin(), ps.end());
    ps.erase(std::unique(ps.begin(), ps.end()), ps.end());
    if (ps.size() < 3)
        return Convex<T>::fromBoundary(std::move(ps));
    std::vector<Point<T>> h;
    h.reserve(ps.size() + 1);
    for (auto p : ps) {
        while (h.size() > 1 and orient(h[h.size() - 2], h.back(), p) <= 0)
            h.pop_back();
        h.push_back(p);
    }
    int bot = int(h.size());
    for (int i = int(ps.size()) - 2; i >= 0; --i) {
        while (int(h.size()) > bot and orient(h[h.size() - 2], h.back(), ps[i]) <= 0)
            h.pop_back();
        h.push_back(ps[i]);
    }
    h.pop_back();
    return Convex<T>::fromBoundary(std::move(h));
}

template <class T>
Hit<T> inter(const Convex<T> &h, const Line<T> &l) {
    static_assert(!std::is_integral_v<T>, "intersection coordinates need floating T");
    auto hit = h.hit(l);
    if (hit.kind == Convex<T>::LineHullKind::NONE)
        return {};
    if (hit.kind == Convex<T>::LineHullKind::VERTEX)
        return {HitKind::ONE, {h[hit.a], {}}};
    if (hit.kind == Convex<T>::LineHullKind::EDGE)
        return inter(l, h.edge(hit.a));
    auto a = inter(l, h.edge(hit.a)).ps[0];
    auto b = inter(l, h.edge(hit.b)).ps[0];
    if (l.v.dot(b - a) < 0)
        std::swap(a, b);
    return {a == b ? HitKind::ONE : HitKind::SEG, {a, b}};
}

template <class T>
Hit<T> inter(const Convex<T> &h, const Seg<T> &s) {
    if (s.a == s.b)
        return h.loc(s.a) == Location::OUT ? Hit<T>{} : Hit<T>{HitKind::ONE, {s.a, {}}};
    auto hit = inter(h, s.line());
    if (!hit)
        return {};
    return inter(Seg<T>{hit.ps[0], hit.size() == 1 ? hit.ps[0] : hit.ps[1]}, s);
}

template <class T>
bool inter(const Convex<T> &h, const Line<T> &l, std::nullptr_t) {
    if (h.empty())
        return false;
    if (h.size() <= 2)
        return inter(l, Seg<T>{h[0], h[h.size() - 1]}, nullptr);
    return l.side(h[h.peak(l.v.rot90())]) >= 0 and l.side(h[h.peak(-l.v.rot90())]) <= 0;
}

template <class T>
bool inter(const Convex<T> &h, const Seg<T> &s, std::nullptr_t) {
    if (h.loc(s.a) != Location::OUT or h.loc(s.b) != Location::OUT)
        return true;
    if (s.a == s.b)
        return false;
    auto hit = h.hit(s.line());
    if (hit.kind == Convex<T>::LineHullKind::NONE)
        return false;
    if (hit.kind == Convex<T>::LineHullKind::VERTEX)
        return (s.loc(h[hit.a]) == Location::ON);
    return inter(s, h.edge(hit.a), nullptr) or inter(s, h.edge(hit.b), nullptr);
}

template <class T>
Convex<T> cutLeft(const Convex<T> &h, const Line<T> &l) {
    static_assert(!std::is_integral_v<T>, "cut coordinates need floating T");
    auto hit = h.hit(l);
    if (hit.kind == Convex<T>::LineHullKind::NONE)
        return h.empty() or l.side(h[0]) < 0 ? Convex<T>{} : h;
    if (h.size() < 3) {
        std::vector<Point<T>> ps;
        for (auto p : h.vertices())
            if (l.side(p) >= 0)
                ps.push_back(p);
        auto q = inter(h, l);
        for (int i = 0; i < q.size(); ++i)
            ps.push_back(q.ps[i]);
        return convexHull(std::move(ps));
    }
    if (hit.kind == Convex<T>::LineHullKind::VERTEX or hit.kind == Convex<T>::LineHullKind::EDGE) {
        if (l.side(h[h.peak(l.v.rot90())]) > 0)
            return h;
        if (hit.kind == Convex<T>::LineHullKind::VERTEX)
            return Convex<T>::fromBoundary({h[hit.a]});
        return Convex<T>::fromBoundary({h[hit.a], h.at(hit.a + 1)});
    }
    int a = hit.a, b = hit.b;
    // Choose the retained CCW chain between the two crossing edges.
    if (l.side(h.at(a + 1)) < 0)
        std::swap(a, b);
    std::vector<Point<T>> ps;
    ps.push_back(inter(l, h.edge(a)).ps[0]);
    for (int i = (a + 1) % h.size(); i != (b + 1) % h.size(); i = (i + 1) % h.size())
        ps.push_back(h[i]);
    ps.push_back(inter(l, h.edge(b)).ps[0]);
    return Convex<T>::fromBoundary(std::move(ps));
}

// Closed polygon-line intersection may have several disconnected components.
template <class T>
std::vector<Seg<T>> inter(const Polygon<T> &p, const Line<T> &l) {
    static_assert(!std::is_integral_v<T>, "intersection coordinates need floating T");
    struct Event {
        T t;
        int dlt;
        Point<T> p;
    };
    std::vector<Event> evs;
    std::vector<Seg<T>> segs;
    auto proj = [&](Point<T> p) {
        return l.v.dot(p - l.p);
    };
    for (int i = 0; i < p.size(); ++i) {
        auto s = p.edge(i);
        int a = l.side(s.a), b = l.side(s.b);
        auto hit = inter(l, s);
        if (!hit)
            continue;
        if (hit.kind == HitKind::SEG)
            segs.push_back({hit.ps[0], hit.ps[1]});
        else
            segs.push_back({hit.ps[0], hit.ps[0]});
        // Half-open crossings avoid counting a vertex twice.
        if ((a <= 0 and b > 0) or (b <= 0 and a > 0))
            evs.push_back({proj(hit.ps[0]), b > a ? 1 : -1, hit.ps[0]});
    }
    std::sort(evs.begin(), evs.end(), [](const Event &a, const Event &b) {
        return a.t < b.t;
    });
    int wn = 0;
    for (int i = 0; i < int(evs.size());) {
        int j = i;
        while (j < int(evs.size()) and evs[j].t == evs[i].t)
            wn += evs[j++].dlt;
        if (wn and j < int(evs.size()))
            segs.push_back({evs[i].p, evs[j].p});
        i = j;
    }
    std::sort(segs.begin(), segs.end(), [&](const Seg<T> &a, const Seg<T> &b) {
        return proj(a.a) < proj(b.a);
    });
    std::vector<Seg<T>> ans;
    for (auto s : segs) {
        if (!ans.empty() and proj(s.a) <= proj(ans.back().b)) {
            if (proj(ans.back().b) < proj(s.b))
                ans.back().b = s.b;
        } else
            ans.push_back(s);
    }
    return ans;
}

template <class T>
std::vector<Seg<T>> inter(const Polygon<T> &p, const Seg<T> &s) {
    if (s.a == s.b)
        return p.loc(s.a) == Location::OUT ? std::vector<Seg<T>>{} : std::vector<Seg<T>>{s};
    std::vector<Seg<T>> ans;
    for (auto part : inter(p, s.line())) {
        auto hit = inter(part, s);
        if (hit)
            ans.push_back({hit.ps[0], hit.size() == 1 ? hit.ps[0] : hit.ps[1]});
    }
    return ans;
}

template <class T>
Near<T> near(Point<T> p, const Convex<T> &h) {
    static_assert(!std::is_integral_v<T>, "nearest coordinates need floating T");
    return {p, h.foot(p)};
}

template <class T>
Near<T> near(const Convex<T> &h, const Line<T> &l) {
    static_assert(!std::is_integral_v<T>, "nearest coordinates need floating T");
    assert(!h.empty());
    if (h.size() <= 2) {
        auto q = near(l, Seg<T>{h[0], h[h.size() - 1]});
        return {q.second, q.first};
    }
    auto hit = h.hit(l);
    if (hit.kind != Convex<T>::LineHullKind::NONE) {
        auto p =
            hit.kind == Convex<T>::LineHullKind::CROSS ? inter(l, h.edge(hit.a)).ps[0] : h[hit.a];
        return {p, p};
    }
    int i = std::abs(l.eval(h[hit.a])) < std::abs(l.eval(h[hit.b])) ? hit.a : hit.b;
    return near(h[i], l);
}

template <class T>
bool inter(Point<T> p, const Polygon<T> &s, std::nullptr_t) {
    return s.loc(p) != Location::OUT;
}

template <class T>
Hit<T> inter(Point<T> p, const Polygon<T> &s) {
    return inter(p, s, nullptr) ? Hit<T>{HitKind::ONE, {p, {}}} : Hit<T>{};
}

template <class T>
bool inter(Point<T> p, const Convex<T> &s, std::nullptr_t) {
    return s.loc(p) != Location::OUT;
}

template <class T>
Hit<T> inter(Point<T> p, const Convex<T> &s) {
    return inter(p, s, nullptr) ? Hit<T>{HitKind::ONE, {p, {}}} : Hit<T>{};
}

template <class T>
bool inter(const Polygon<T> &p, const Line<T> &s, std::nullptr_t) {
    for (int i = 0; i < p.size(); ++i)
        if (inter(Seg<T>{p.ps[i], p.ps[(i + 1) % p.size()]}, s, nullptr))
            return true;
    return false;
}

template <class T>
bool inter(const Polygon<T> &p, const Seg<T> &s, std::nullptr_t) {
    if (p.loc(s.a) != Location::OUT or p.loc(s.b) != Location::OUT)
        return true;
    for (int i = 0; i < p.size(); ++i)
        if (inter(Seg<T>{p.ps[i], p.ps[(i + 1) % p.size()]}, s, nullptr))
            return true;
    return false;
}

template <class T>
Near<T> near(Point<T> q, const Polygon<T> &p) {
    static_assert(!std::is_integral_v<T>, "nearest coordinates need floating T");
    assert(p.size());
    if (p.loc(q) != Location::OUT)
        return {q, q};
    auto best = near(q, p.ps[0]);
    for (int i = 0; i < p.size(); ++i) {
        auto cand = near(q, Seg<T>{p.ps[i], p.ps[(i + 1) % p.size()]});
        if (q.dist2(cand.second) < q.dist2(best.second))
            best = cand;
    }
    return best;
}

template <class T>
Near<T> near(const Polygon<T> &p, const Seg<T> &s) {
    assert(p.size());
    if (p.loc(s.a) != Location::OUT)
        return {s.a, s.a};
    if (p.loc(s.b) != Location::OUT)
        return {s.b, s.b};
    const auto &ps = p.ps;
    auto best = near(ps[0], s);
    for (int i = 0; i < p.size(); ++i) {
        auto q = near(Seg<T>{ps[i], ps[(i + 1) % p.size()]}, s);
        if (q.first.dist2(q.second) < best.first.dist2(best.second))
            best = q;
        if (best.first == best.second)
            break;
    }
    return best;
}

template <class T>
Near<T> near(const Polygon<T> &p, const Line<T> &s) {
    assert(p.size());
    const auto &ps = p.ps;
    auto best = near(ps[0], s);
    for (int i = 0; i < p.size(); ++i) {
        auto q = near(Seg<T>{ps[i], ps[(i + 1) % p.size()]}, s);
        if (q.first.dist2(q.second) < best.first.dist2(best.second))
            best = q;
        if (best.first == best.second)
            break;
    }
    return best;
}

template <class T>
Near<T> near(const Convex<T> &p, const Seg<T> &s) {
    assert(p.size());
    if (p.loc(s.a) != Location::OUT)
        return {s.a, s.a};
    if (p.loc(s.b) != Location::OUT)
        return {s.b, s.b};
    const auto &ps = p.vertices();
    auto best = near(ps[0], s);
    for (int i = 0; i < p.size(); ++i) {
        auto q = near(Seg<T>{ps[i], ps[(i + 1) % p.size()]}, s);
        if (q.first.dist2(q.second) < best.first.dist2(best.second))
            best = q;
        if (best.first == best.second)
            break;
    }
    return best;
}

template <class T>
bool inter(const Polygon<T> &a, const Polygon<T> &b, std::nullptr_t) {
    if (!a.size() or !b.size())
        return false;
    const auto &pa = a.ps, &pb = b.ps;
    if (a.loc(pb[0]) != Location::OUT or b.loc(pa[0]) != Location::OUT)
        return true;
    for (int i = 0; i < a.size(); ++i)
        for (int j = 0; j < b.size(); ++j)
            if (inter(Seg<T>{pa[i], pa[(i + 1) % a.size()]}, Seg<T>{pb[j], pb[(j + 1) % b.size()]},
                      nullptr))
                return true;
    return false;
}

template <class T>
Near<T> near(const Polygon<T> &a, const Polygon<T> &b) {
    assert(a.size() and b.size());
    const auto &pa = a.ps, &pb = b.ps;
    if (a.loc(pb[0]) != Location::OUT)
        return {pb[0], pb[0]};
    if (b.loc(pa[0]) != Location::OUT)
        return {pa[0], pa[0]};
    auto best = near(pa[0], pb[0]);
    for (int i = 0; i < a.size(); ++i)
        for (int j = 0; j < b.size(); ++j) {
            auto q =
                near(Seg<T>{pa[i], pa[(i + 1) % a.size()]}, Seg<T>{pb[j], pb[(j + 1) % b.size()]});
            if (q.first.dist2(q.second) < best.first.dist2(best.second))
                best = q;
            if (best.first == best.second)
                return best;
        }
    return best;
}

template <class T>
bool inter(const Polygon<T> &a, const Convex<T> &b, std::nullptr_t) {
    if (!a.size() or !b.size())
        return false;
    const auto &pa = a.ps, &pb = b.vertices();
    if (a.loc(pb[0]) != Location::OUT or b.loc(pa[0]) != Location::OUT)
        return true;
    for (int i = 0; i < a.size(); ++i)
        for (int j = 0; j < b.size(); ++j)
            if (inter(Seg<T>{pa[i], pa[(i + 1) % a.size()]}, Seg<T>{pb[j], pb[(j + 1) % b.size()]},
                      nullptr))
                return true;
    return false;
}

template <class T>
Near<T> near(const Polygon<T> &a, const Convex<T> &b) {
    assert(a.size() and b.size());
    const auto &pa = a.ps, &pb = b.vertices();
    if (a.loc(pb[0]) != Location::OUT)
        return {pb[0], pb[0]};
    if (b.loc(pa[0]) != Location::OUT)
        return {pa[0], pa[0]};
    auto best = near(pa[0], pb[0]);
    for (int i = 0; i < a.size(); ++i)
        for (int j = 0; j < b.size(); ++j) {
            auto q =
                near(Seg<T>{pa[i], pa[(i + 1) % a.size()]}, Seg<T>{pb[j], pb[(j + 1) % b.size()]});
            if (q.first.dist2(q.second) < best.first.dist2(best.second))
                best = q;
            if (best.first == best.second)
                return best;
        }
    return best;
}

GEO2_REVERSE(Convex, Point)
GEO2_REVERSE_INTER(Convex, Point)
GEO2_REVERSE(Polygon, Point)
GEO2_REVERSE_INTER(Polygon, Point)

GEO2_REVERSE(Line, Convex)
GEO2_REVERSE_INTER(Line, Convex)
GEO2_REVERSE(Seg, Convex)
GEO2_REVERSE_INTER(Seg, Convex)
GEO2_REVERSE(Line, Polygon)
GEO2_REVERSE_INTER(Line, Polygon)
GEO2_REVERSE(Seg, Polygon)
GEO2_REVERSE_INTER(Seg, Polygon)

GEO2_REVERSE(Convex, Polygon)

// O(n). Empty input has no pair; a singleton pairs with itself.
template <class T>
std::optional<std::array<int, 2>> farthestPair(const Convex<T> &h) {
    int n = h.size();
    if (!n)
        return std::nullopt;
    std::array<int, 2> ans{0, n == 1 ? 0 : 1};
    T best = h[ans[0]].dist2(h[ans[1]]);
    auto upd = [&](int a, int b) {
        T d = h[a].dist2(h[b]);
        if (d > best)
            best = d, ans = {a, b};
    };
    int j = 1;
    for (int i = 0; i < n and n > 2; ++i) {
        auto v = h.edge(i).vec();
        T step;
        while ((step = v.cross(h.at(j + 1) - h[j])) > 0)
            j = j + 1 == n ? 0 : j + 1;
        upd(i, j);
        upd((i + 1) % n, j);
        if (step == 0) {
            upd(i, (j + 1) % n);
            upd((i + 1) % n, (j + 1) % n);
        }
    }
    return ans;
}

template <class T>
auto diameter(const Convex<T> &h) {
    auto pair = farthestPair(h);
    assert(pair);
    return h[(*pair)[0]].dist(h[(*pair)[1]]);
}

// O(n). Division is done in the naturally inferred metric type.
template <class T>
auto minWidth(const Convex<T> &h) {
    using R = decltype(Vec<T>{}.len());
    if (h.size() < 3)
        return R(0);
    R best = h.edge(0).len();
    int j = 1;
    for (int i = 0; i < h.size(); ++i) {
        auto v = h.edge(i).vec();
        while (v.cross(h.at(j + 1) - h[j]) > 0)
            j = j + 1 == h.size() ? 0 : j + 1;
        R wid = R(v.cross(h[j] - h[i])) / v.len();
        if (i == 0 or wid < best)
            best = wid;
    }
    return best;
}

template <class T>
struct BoundingRect {
    std::array<Point<T>, 4> ps{};
    T area{}, perimeter{};
};

// O(n). Three monotone calipers track cross, maximum dot, and minimum dot.
template <class T>
std::optional<BoundingRect<T>> minBoundingRect(const Convex<T> &h) {
    static_assert(!std::is_integral_v<T>, "rectangle corners need floating T");
    int n = h.size();
    if (!n)
        return std::nullopt;
    BoundingRect<T> ans;
    if (n <= 2) {
        ans.ps = {h[0], h[n - 1], h[n - 1], h[0]};
        ans.perimeter = h[0].dist(h[n - 1]) * T(2);
        return ans;
    }
    auto first = h.edge(0).vec();
    int top = h.peak(first.rot90()), rhs = h.peak(first), left = h.peak(-first);
    for (int i = 0; i < n; ++i) {
        auto v = h.edge(i).vec();
        while (v.cross(h.at(top + 1) - h[top]) > 0)
            top = top + 1 == n ? 0 : top + 1;
        while (v.dot(h.at(rhs + 1) - h[rhs]) > 0)
            rhs = rhs + 1 == n ? 0 : rhs + 1;
        while (v.dot(h.at(left + 1) - h[left]) < 0)
            left = left + 1 == n ? 0 : left + 1;
        T lo = v.dot(h[left] - h[i]), hi = v.dot(h[rhs] - h[i]);
        T hgt = v.cross(h[top] - h[i]), d2 = v.len2();
        T area = (hi - lo) * hgt / d2;
        if (i and area >= ans.area)
            continue;
        auto a = h[i] + v * (lo / d2), b = h[i] + v * (hi / d2);
        auto w = v.rot90() * (hgt / d2);
        ans = {{a, b, b + w, a + w}, area, T(2) * (hi - lo + hgt) / v.len()};
    }
    return ans;
}

// O(n+m). Keep original vertex pairs; emit returns false to stop early.
template <class T>
template <class Emit>
void Convex<T>::walk(const Convex &a, const Convex &b, bool sub, Emit emit) {
    if (a.empty() or b.empty())
        return;
    auto vert = [&](int i) {
        return sub ? Point<T>{-b[i].x, -b[i].y} : b[i];
    };
    auto less = [](Point<T> p, Point<T> q) {
        return p.y != q.y ? p.y < q.y : p.x < q.x;
    };
    int x = 0, y = 0;
    for (int i = 1; i < a.size(); ++i)
        if (less(a[i], a[x]))
            x = i;
    for (int i = 1; i < b.size(); ++i)
        if (less(vert(i), vert(y)))
            y = i;
    int i = 0, j = 0;
    while (i < a.size() or j < b.size()) {
        int ai = x + i, bj = y + j;
        if (ai >= a.size())
            ai -= a.size();
        if (bj >= b.size())
            bj -= b.size();
        if (!emit(ai, bj))
            return;
        if (i == a.size()) {
            ++j;
            continue;
        }
        if (j == b.size()) {
            ++i;
            continue;
        }
        auto u = a.at(ai + 1) - a[ai], v = b.at(bj + 1) - b[bj];
        if (sub)
            v = -v;
        if (u == Vec<T>::O) {
            ++i;
            continue;
        }
        if (v == Vec<T>::O) {
            ++j;
            continue;
        }
        int hu = u.half(), hv = v.half();
        if (hu != hv) {
            if (hu < hv)
                ++i;
            else
                ++j;
        } else {
            T c = u.cross(v);
            if (c > 0)
                ++i;
            else if (c < 0)
                ++j;
            else
                ++i, ++j;
        }
    }
}

template <class T>
Convex<T> minkowskiSum(const Convex<T> &a, const Convex<T> &b) {
    std::vector<Point<T>> ps;
    ps.reserve(a.size() + b.size());
    Convex<T>::walk(a, b, false, [&](int i, int j) {
        ps.push_back(a[i] + b[j].toVec());
        return true;
    });
    return Convex<T>::fromBoundary(std::move(ps));
}

template <class T>
Convex<T> minkowskiDiff(const Convex<T> &a, const Convex<T> &b) {
    std::vector<Point<T>> ps;
    ps.reserve(a.size() + b.size());
    Convex<T>::walk(a, b, true, [&](int i, int j) {
        ps.push_back(a[i] - b[j].toVec());
        return true;
    });
    return Convex<T>::fromBoundary(std::move(ps));
}

template <class T>
bool inter(const Convex<T> &a, const Convex<T> &b, std::nullptr_t) {
    if (a.empty() or b.empty())
        return false;
    Point<T> first{}, prev{};
    bool beg = false, in = true, turn = false, on = false;
    auto edge = [&](Point<T> p, Point<T> q) {
        int c = orient(Point<T>::O, p, q);
        in &= c >= 0;
        turn |= c != 0;
        on |= c == 0 and p.toVec().dot(q.toVec()) <= 0;
    };
    Convex<T>::walk(a, b, true, [&](int i, int j) {
        auto p = a[i] - b[j].toVec();
        if (beg)
            edge(prev, p);
        else
            first = p, beg = true;
        prev = p;
        return !on;
    });
    edge(prev, first);
    return on or (in and turn);
}

// O(n+m). Barycentric witnesses recover a common point when the origin is inside.
template <class T>
Near<T> near(const Convex<T> &a, const Convex<T> &b) {
    static_assert(!std::is_integral_v<T>, "nearest coordinates need floating T");
    assert(!a.empty() and !b.empty());
    Near<T> first{}, prev{}, best{};
    bool beg = false, same = false;
    T gap{};
    auto diff = [](Near<T> pair) {
        auto v = pair.first - pair.second;
        return Point<T>{v.x, v.y};
    };
    auto edge = [&](Near<T> pair) {
        if (same)
            return;
        auto base = diff(first), p = diff(prev), q = diff(pair);
        auto u = p - base, v = q - base;
        T d = u.cross(v);
        if (d != 0 and d > 0 and orient(base, p, Point<T>::O) >= 0 and
            orient(p, q, Point<T>::O) >= 0 and orient(q, base, Point<T>::O) >= 0) {
            T x = (-base.toVec()).cross(v) / d, y = u.cross(-base.toVec()) / d;
            auto pt = first.first + (prev.first - first.first) * x + (pair.first - first.first) * y;
            best = {pt, pt};
            same = true;
            return;
        }
        auto e = q - p;
        T t = e == Vec<T>::O ? T(0) : std::clamp(e.dot(-p.toVec()) / e.len2(), T(0), T(1));
        auto a = prev.first + (pair.first - prev.first) * t;
        auto b = prev.second + (pair.second - prev.second) * t;
        T value = a.dist2(b);
        if (value < gap)
            best = {a, b}, gap = value;
    };
    Convex<T>::walk(a, b, true, [&](int i, int j) {
        Near<T> pair{a[i], b[j]};
        if (beg)
            edge(pair);
        else
            first = best = pair, gap = pair.first.dist2(pair.second), beg = true;
        prev = pair;
        return !same;
    });
    edge(first);
    return best;
}

// O(n). The resulting directed edges are sorted by polar angle.
template <class T>
std::vector<Line<T>> Convex<T>::cuts() const {
    const auto &h = *this;
    assert(h.size() >= 3);
    std::vector<Line<T>> ls;
    ls.reserve(h.size());
    for (int i = 0; i < h.size(); ++i)
        ls.push_back(h.edge(i).line());
    auto first = std::min_element(ls.begin(), ls.end(), [](const Line<T> &a, const Line<T> &b) {
        return cmp(a.v, b.v);
    });
    std::rotate(ls.begin(), first, ls.end());
    return ls;
}

// Input lines must be sorted by polar angle. A finite convex domain is explicit.
// O(n+b) normally. Parallel/degenerate cases fall back to boundary clipping,
// O(n(n+b)) worst case. Empty/point/segment results are valid.
template <class T>
Convex<T> Convex<T>::hpi(const std::vector<Line<T>> &ls, const Convex<T> &box) {
    static_assert(!std::is_integral_v<T>, "half-plane coordinates need floating T");
    auto slow = [&] {
        auto h = box;
        for (const auto &l : ls) {
            h = cutLeft(h, l);
            if (h.empty())
                break;
        }
        return h;
    };
    if (box.size() < 3)
        return slow();
    auto es = box.cuts();
    std::vector<Line<T>> all;
    all.reserve(ls.size() + es.size());
    std::merge(ls.begin(), ls.end(), es.begin(), es.end(), std::back_inserter(all),
               [](const Line<T> &a, const Line<T> &b) {
                   return cmp(a.v, b.v);
               });
    std::vector<Line<T>> uniq;
    uniq.reserve(all.size());
    for (const auto &l : all) {
        if (!uniq.empty() and uniq.back().parallel(l) and uniq.back().v.dot(l.v) > 0) {
            if (uniq.back().side(l.p) > 0)
                uniq.back() = l;
        } else
            uniq.push_back(l);
    }
    // Integer-valued floating input admits exact topology within the input bounds.
    auto ival = [](T x) {
        if constexpr (std::is_arithmetic_v<T>)
            return static_cast<long long>(x);
        else
            return std::llround(x.val());
    };
    auto fits = [&](T x, T lim) {
        if (std::abs(x) > lim)
            return false;
        T rnd = T(ival(x));
        return !(x < rnd or x > rnd);
    };
    bool safe = true;
    for (const auto &l : uniq) {
        if (!fits(l.p.x, T(1E9)) or !fits(l.p.y, T(1E9)) or !fits(l.v.x, T(2E9)) or
            !fits(l.v.y, T(2E9))) {
            safe = false;
            break;
        }
    }
    auto lint = [&](const Line<T> &l) {
        return Line<long long>::fromVec({ival(l.p.x), ival(l.p.y)}, {ival(l.v.x), ival(l.v.y)});
    };
    // Opposite coincident constraints reduce the result to this original line.
    // Clip its parameter interval without rebuilding rounded polygon edges.
    auto onl = [&](const Line<T> &l) {
        std::optional<T> lo, hi;
        for (const auto &h : uniq) {
            T c = h.v.cross(l.v), d = h.eval(l.p);
            if (c == 0) {
                if (h.side(l.p) < 0)
                    return Convex{};
                continue;
            }
            T t = -d / c;
            if (c > 0) {
                if (!lo or t > *lo)
                    lo = t;
            } else {
                if (!hi or t < *hi)
                    hi = t;
            }
        }
        assert(lo and hi);
        if (*lo > *hi)
            return Convex{};
        return Convex::fromBoundary({l.p + l.v * *lo, l.p + l.v * *hi});
    };
    int cut = 0;
    while (cut < int(uniq.size()) and uniq[cut].v.half() == 0)
        ++cut;
    int opp = cut;
    for (int i = 0; i < cut; ++i) {
        auto aim = -uniq[i].v;
        while (opp < int(uniq.size()) and cmp(uniq[opp].v, aim))
            ++opp;
        for (int j : {opp - 1, opp}) {
            if (j < cut or j >= int(uniq.size()) or !uniq[i].parallel(uniq[j]))
                continue;
            int side = uniq[i].side(uniq[j].p);
            if (side < 0)
                return Convex{};
            if (!side)
                return onl(uniq[i]);
        }
    }
    std::vector<Line<T>> q;
    q.reserve(uniq.size());
    size_t head = 0;
    auto out = [&](const Line<T> &l, const Line<T> &a, const Line<T> &b) {
        if (safe) {
            auto u = lint(a), v = lint(b), h = lint(l);
            auto d = u.v.cross(v.v), x = h.eval(u.p), y = h.v.cross(u.v);
            auto t = (v.p - u.p).cross(v.v);
            __int128_t value = __int128_t(x) * d + __int128_t(y) * t;
            return value != 0 and (value < 0) != (d < 0);
        }
        return l.side(inter(a, b).ps[0]) < 0;
    };
    for (const auto &l : uniq) {
        while (q.size() - head > 1) {
            if (q[q.size() - 2].parallel(q.back()))
                return slow();
            if (!out(l, q[q.size() - 2], q.back()))
                break;
            q.pop_back();
        }
        while (q.size() - head > 1) {
            if (q[head].parallel(q[head + 1]))
                return slow();
            if (!out(l, q[head], q[head + 1]))
                break;
            ++head;
        }
        if (q.size() > head and q.back().parallel(l))
            return slow();
        q.push_back(l);
    }
    while (q.size() - head > 2 and !q[q.size() - 2].parallel(q.back()) and
           out(q[head], q[q.size() - 2], q.back()))
        q.pop_back();
    while (q.size() - head > 2 and !q[head].parallel(q[head + 1]) and
           out(q.back(), q[head], q[head + 1]))
        ++head;
    if (q.size() - head < 3)
        return slow();
    if (q[head].parallel(q.back()))
        return slow();
    std::vector<Point<T>> ps;
    ps.reserve(q.size() - head);
    for (size_t i = head; i < q.size(); ++i) {
        size_t j = i + 1 == q.size() ? head : i + 1;
        if (q[i].parallel(q[j]))
            return slow();
        ps.push_back(inter(q[i], q[j]).ps[0]);
    }
    auto h = Convex<T>::fromBoundary(std::move(ps));
    if (h.size() < 3 and safe) {
        for (const auto &l : uniq)
            if (out(l, q[head], q[head + 1]))
                return Convex{};
    }
    return h;
}

// O(n log n + b), with the same documented degenerate fallback.
template <class T>
Convex<T> halfPlaneIntersection(std::vector<Line<T>> ls, const Convex<T> &box) {
    std::sort(ls.begin(), ls.end(), [](const Line<T> &a, const Line<T> &b) {
        return Convex<T>::cmp(a.v, b.v);
    });
    return Convex<T>::hpi(ls, box);
}

// The finite domain is a itself. Degenerate inputs use segment clipping.
template <class T>
Convex<T> inter(const Convex<T> &a, const Convex<T> &b) {
    if (a.empty() or b.empty())
        return {};
    if (b.size() < 3) {
        auto hit = inter(a, Seg<T>{b[0], b[b.size() - 1]});
        std::vector<Point<T>> ps;
        for (int i = 0; i < hit.size(); ++i)
            ps.push_back(hit.ps[i]);
        return Convex<T>::fromBoundary(std::move(ps));
    }
    return Convex<T>::hpi(b.cuts(), a);
}

} // namespace _geo2

using _geo2::Polygon;
using _geo2::Convex;
using _geo2::BoundingRect;
using _geo2::inter;
using _geo2::near;
using _geo2::convexHull;
using _geo2::cutLeft;
using _geo2::farthestPair;
using _geo2::diameter;
using _geo2::minWidth;
using _geo2::minBoundingRect;
using _geo2::minkowskiSum;
using _geo2::minkowskiDiff;
using _geo2::halfPlaneIntersection;
// SNIPPET END
