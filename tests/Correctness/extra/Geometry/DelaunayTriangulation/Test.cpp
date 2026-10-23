#include "../../../../../src/extra/Geometry/DelaunayTriangulation/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("DelaunayTriangulation/oracle", [] {
        using P = Point<long double>;
        using Triangle = std::array<int, 3>;
        auto cross = [](P a, P b, P c) {
            return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        };
        auto circle = [](P a, P b, P c, P d) {
            a.x -= d.x;
            a.y -= d.y;
            b.x -= d.x;
            b.y -= d.y;
            c.x -= d.x;
            c.y -= d.y;
            return (a.x * a.x + a.y * a.y) * (b.x * c.y - b.y * c.x)
                 - (b.x * b.x + b.y * b.y) * (a.x * c.y - a.y * c.x)
                 + (c.x * c.x + c.y * c.y) * (a.x * b.y - a.y * b.x);
        };
        auto check = [&](const std::vector<P>& ps) {
            int n = int(ps.size());
            if (n < 3) {
                Delaunay d(ps);
                CHECK(d.face.empty());
                return;
            }

            // Independent exhaustive oracle: in general position, exactly the
            // empty-circumcircle triples form the unique Delaunay triangulation.
            std::set<Triangle> expected;
            for (int a = 0; a < n; ++a) {
                for (int b = a + 1; b < n; ++b) {
                    for (int c = b + 1; c < n; ++c) {
                        auto sign = cross(ps[a], ps[b], ps[c]);
                        CHECK(sign != 0);
                        bool empty = true;
                        for (int k = 0; k < n; ++k) {
                            if (k == a or k == b or k == c) {
                                continue;
                            }
                            auto det = circle(ps[a], ps[b], ps[c], ps[k]);
                            CHECK(det != 0);
                            empty = empty and det * sign < 0;
                        }
                        if (empty) {
                            expected.insert({a, b, c});
                        }
                    }
                }
            }

            int hull = 0;
            for (int i = 0; i < n; ++i) {
                for (int j = i + 1; j < n; ++j) {
                    bool pos = false, neg = false;
                    for (int k = 0; k < n; ++k) {
                        auto v = cross(ps[i], ps[j], ps[k]);
                        pos = pos or v > 0;
                        neg = neg or v < 0;
                    }
                    hull += !(pos and neg);
                }
            }

            Delaunay d(ps);
            std::set<Triangle> actual;
            std::vector<bool> covered(n);
            for (auto e : d.face) {
                CHECK(e and e->r and e->r->r and e->r->r->r == e);
                auto p = e;
                for (int i = 0; i < 3; ++i) {
                    CHECK(p->inv and p->inv->inv == p and p->r->l == p);
                    CHECK(0 <= p->from and p->from < n + 3);
                    p = p->r;
                }
                int a = e->from, b = e->r->from, c = e->r->r->from;
                if (a >= n or b >= n or c >= n) {
                    continue;
                }
                auto sign = cross(ps[a], ps[b], ps[c]);
                CHECK(sign != 0);
                for (int k = 0; k < n; ++k) {
                    if (k != a and k != b and k != c) {
                        CHECK(circle(ps[a], ps[b], ps[c], ps[k]) * sign < 0);
                    }
                }
                Triangle key{a, b, c};
                std::sort(key.begin(), key.end());
                CHECK(actual.insert(key).second);
                covered[a] = covered[b] = covered[c] = true;
            }
            CHECK(actual == expected);
            CHECK(int(actual.size()) == 2 * n - 2 - hull);
            CHECK(std::all_of(covered.begin(), covered.end(), [](bool used) { return used; }));
        };

        check({});
        check({{0, 0}});
        check({{0, 0}, {1, 1}});
        for (int n = 3; n <= 12; ++n) {
            std::vector<P> ps;
            for (int x = 1; x <= n; ++x) {
                ps.emplace_back(x, x * x);
            }
            std::shuffle(ps.begin(), ps.end(), testRng);
            check(ps);
        }

        // One strict interior point, including insertion before any hull point.
        const std::vector<P> inner{{0, 0}, {8, 0}, {0, 7}, {2, 2}};
        std::array<int, 4> order{0, 1, 2, 3};
        do {
            std::vector<P> ps;
            for (int i : order) {
                ps.push_back(inner[i]);
            }
            check(ps);
        } while (std::next_permutation(order.begin(), order.end()));

        // Integer coordinates keep the independent determinants exact here.
        // Reject collinear triples and cocircular quadruples explicitly, rather
        // than relying on random inputs to happen to satisfy the contract.
        std::mt19937_64 rng(20261024);
        std::vector<P> ps{{0, 0}, {1000, 0}, {0, 1000}};
        for (int tries = 0; ps.size() < 12 and tries < 1000; ++tries) {
            int x = 1 + rng() % 399;
            int y = 1 + rng() % 399;
            P p(x, y);
            bool valid = true;
            for (int i = 0; i < int(ps.size()); ++i) {
                for (int j = i + 1; j < int(ps.size()); ++j) {
                    valid = valid and cross(ps[i], ps[j], p) != 0;
                    for (int k = j + 1; k < int(ps.size()); ++k) {
                        valid = valid and circle(ps[i], ps[j], ps[k], p) != 0;
                    }
                }
            }
            if (valid) {
                ps.push_back(p);
            }
        }
        CHECK(ps.size() == 12);
        check(ps);
        std::reverse(ps.begin(), ps.end());
        check(ps);
        for (int i = 0; i < 4; ++i) {
            std::shuffle(ps.begin(), ps.end(), rng);
            check(ps);
        }
    });
    return 0;
}
