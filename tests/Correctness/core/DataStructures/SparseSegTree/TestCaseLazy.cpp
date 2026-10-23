#include "../../../../../src/core/DataStructures/SparseSegTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace lazy_test {
using I = __int128_t;
using C = long long;
struct Tag {
    I add = 0, slope = 0;
    void apply(const Tag &v) { add += v.add; slope += v.slope; }
};
struct Info {
    I sum = 0;
    C l = 0, r = 0;
    Info operator+(const Info &v) const { return {sum + v.sum}; }
    Info operator-(const Info &v) const { return {sum - v.sum}; }
    void apply(const Tag &t, C a, C b) {
        const I len = I(b) - a;
        sum += t.add * len + t.slope * (I(a) + b - 1) * len / 2;
        l = a; r = b;
    }
};
using Tree = SparseSegTree<Info, C, Tag>;
using Arr = std::vector<I>;
static long long checks = 0;
static void need(bool ok) {
    ++checks;
    CHECK(ok);
}
static I sum(const Arr &a, int l, int r) {
    I s = 0;
    for (int i = l; i < r; ++i) s += a[i];
    return s;
}
template <class V>
static void verify(const V &v, const Arr &a, C lo) {
    const int n = int(a.size());
    for (int l = 0; l < n; ++l) {
        for (int r = l + 1; r <= n; ++r) {
            const auto val = v.query(lo + l, lo + r);
            need(val.sum == sum(a, l, r));
            need(val.l == lo + l and val.r == lo + r);
        }
    }
}
static void upd(Tree &t, Arr &a, C lo, int l, int r, Tag tag) {
    t.modify(lo + l, lo + r, tag);
    for (int i = l; i < r; ++i) a[i] += tag.add + tag.slope * (lo + i);
}
static void exhaustive() {
    const C lo = -2;
    constexpr int n = 5;
    Tree::clearInit(lo, lo + n);
    for (int l = 0; l <= n; ++l) {
        for (int r = l; r <= n; ++r) {
            for (int d : {-1, 0, 2}) {
                Tree first;
                Arr a(n);
                upd(first, a, lo, l, r, Tag{d});
                const auto view = first.view();
                for (int ql = 0; ql <= n; ++ql) {
                    for (int qr = ql; qr <= n; ++qr) {
                        for (int e : {-1, 0, 2}) {
                            Tree b = first;
                            Arr ab = a;
                            upd(b, ab, lo, ql, qr, Tag{e});
                            verify(b, ab, lo);
                            verify(first, a, lo);
                            verify(view, a, lo);
                            Tree merged = b;
                            merged.merge(first);
                            Arr both = ab;
                            for (int i = 0; i < n; ++i) both[i] += a[i];
                            verify(merged, both, lo);
                            Arr diff = ab;
                            for (int i = 0; i < n; ++i) diff[i] -= a[i];
                            verify(b - first, diff, lo);
                        }
                    }
                }
            }
        }
    }
}
static void randomMixed() {
    constexpr int n = 17;
    const C lo = -8;
    Tree::clearInit(lo, lo + n);
    std::array<Tree, 8> trees;
    std::array<Arr, 8> vals;
    for (auto &a : vals) a.assign(n, 0);
    using Snap = std::pair<Tree::View<1>, Arr>;
    std::vector<Snap> snaps;
    std::mt19937 rng(static_cast<unsigned>(testSeed) ^ 0x51A2B3C4U);
    for (int it = 0; it < 3500; ++it) {
        const int i = int(rng() % 8), j = int(rng() % 8);
        int l = int(rng() % (n + 1)), r = int(rng() % (n + 1));
        if (l > r) std::swap(l, r);
        switch (rng() % 8) {
        case 0:
            upd(trees[i], vals[i], lo, l, r, Tag{int(rng() % 9) - 4, int(rng() % 3) - 1});
            break;
        case 1: {
            const int x = int(rng() % n);
            const I v = int(rng() % 21) - 10;
            trees[i].modify(lo + x, Info{v});
            vals[i][x] = v;
            break;
        }
        case 2: {
            const int x = int(rng() % n), d = int(rng() % 11) - 5;
            trees[i].modify(lo + x, [d, x, lo](const Info &v) -> Info {
                need(v.l == lo + x and v.r == lo + x + 1);
                return {v.sum + d};
            });
            vals[i][x] += d;
            break;
        }
        case 3:
            trees[i] = trees[j]; vals[i] = vals[j]; break;
        case 4:
            if (i != j) {
                trees[i].merge(trees[j]);
                for (int x = 0; x < n; ++x) vals[i][x] += vals[j][x];
            }
            break;
        case 5:
            if (i != j) {
                const auto old = trees[j].view();
                const Arr a = vals[j];
                trees[i].merge(std::move(trees[j]));
                for (int x = 0; x < n; ++x) vals[i][x] += vals[j][x];
                vals[j].assign(n, 0);
                verify(old, a, lo);
            }
            break;
        case 6:
            if (snaps.size() < 150) snaps.emplace_back(trees[i].view(), vals[i]);
            else {
                trees[i].modify(lo, Info{0});
                vals[i][0] = 0;
            }
            break;
        default:
            trees[i] = Tree{}; vals[i].assign(n, 0); break;
        }
        verify(trees[i], vals[i], lo);
        verify(trees[j], vals[j], lo);
        Arr a = vals[i];
        for (int x = 0; x < n; ++x) a[x] -= vals[j][x];
        verify(trees[i] - trees[j], a, lo);
        // 多根 View + View 拼接与正负号传播。
        a = vals[i];
        for (int x = 0; x < n; ++x) a[x] = vals[i][x] + vals[j][x] - vals[0][x] - vals[1][x];
        verify((trees[i] + trees[j]) - (trees[0] + trees[1]), a, lo);
        if (!snaps.empty()) {
            const auto &snap = snaps[rng() % snaps.size()];
            verify(snap.first, snap.second, lo);
        }
    }
    for (const auto &snap : snaps) verify(snap.first, snap.second, lo);
}
template <class V>
static void verifySearch(const V &v, const Arr &a, C lo) {
    const int n = int(a.size());
    for (int l = 0; l <= n; ++l) {
        for (int r = l; r <= n; ++r) {
            for (I k = 0; k <= sum(a, l, r) + 1; ++k) {
                std::optional<C> f, b;
                I s = 0;
                for (int x = l; x < r; ++x) {
                    s += a[x]; if (s >= k) { f = lo + x; break; }
                }
                s = 0;
                for (int x = r - 1; x >= l; --x) {
                    s += a[x]; if (s >= k) { b = lo + x; break; }
                }
                need(v.first(lo+l, lo+r, [k, l, r, lo](const Info &cur) {
                    need(cur.l == lo + l and cur.l < cur.r and cur.r <= lo + r);
                    return cur.sum >= k;
                }) == f);
                need(v.last(lo+l, lo+r, [k, l, r, lo](const Info &cur) {
                    need(cur.r == lo + r and lo + l <= cur.l and cur.l < cur.r);
                    return cur.sum >= k;
                }) == b);
            }
        }
    }
}
static void searchCases() {
    constexpr int n = 13;
    const C lo = -4;
    Tree::clearInit(lo, lo + n);
    Tree t;
    Arr a(n);
    upd(t, a, lo, 0, n, Tag{1});
    upd(t, a, lo, 2, 8, Tag{2});
    t.modify(lo + 6, Info{0}); a[6] = 0;
    verifySearch(t, a, lo);
    need(t.first(lo, lo + n, [k = std::make_unique<I>(4)](const Info &v) {
        return v.sum >= *k;
    }) == lo + 2);
    need(t.last(lo, lo + n, [k = std::make_unique<I>(4)](const Info &v) {
        return v.sum >= *k;
    }) == lo + 9);
}
static void nestedViews() {
    const C lo = -2;
    Tree::clearInit(lo, lo + 5, 1);
    Tree a, b, c, d;
    a.modify(lo, lo + 5, Tag{4}).modify(0, 1, Tag{1});
    b.modify(lo, lo + 5, Tag{2});
    c.modify(lo, lo + 5, Tag{1}).modify(-1, 2, Tag{2});
    d.modify(lo, lo + 5, Tag{1});
    const Arr expected{2, 0, 1, 0, 2};
    const auto check = [&](const auto &v) {
        verify(v, expected, lo);
        verifySearch(v, expected, lo);
    };
    // Subtracting an already signed View must flip all its existing signs.
    const auto nested = (a - b) - (c - d);
    check(nested);
    check(a + (d - c) - b);
    check(a - (b + c - d));
    Tree::View<0> empty;
    check((empty - (b - a)) - (c - d));
    const Arr zero(5);
    verify(empty, zero, lo);
    verifySearch(empty, zero, lo);
    verify(nested - nested, zero, lo);
    verifySearch(nested - nested, zero, lo);
    const Arr av{4, 4, 5, 4, 4};
    verify(empty + a, av, lo);
    verifySearch(empty + a, av, lo);
    verify(empty - (empty - a), av, lo); // Signed View with one positive root.
    verifySearch(empty - (empty - a), av, lo);
    a.modify(0, Info{99});
    d.modify(lo, lo + 5, Tag{7});
    check(nested); // A stored multi-root View owns a snapshot of every source.
}
static void extremes() {
    const C up = 1'000'000'000'000'000'000LL;
    Tree::clearInit(-up, up);
    Tree t;
    t.modify(-up, up, Tag{3});
    auto old = t.view();
    need(t.query(-up, up).sum == I(6) * up);
    t.modify(-up, Info{9});
    t.modify(up-1, Info{-2});
    need(t.query(-up, up).sum == I(6) * up + 1);
    need(old.query(-up, up).sum == I(6) * up);
    need(t.query(-123, 456).sum == I(579) * 3);
    Tree u;
    u.modify(-up, up, Tag{2, 1});
    auto before = u.view();
    t.merge(std::move(u));
    need(t.query(-up, up).sum == I(9) * up + 1);
    need(before.query(-up, up).sum == I(3) * up);
    need(u.query(-up, up).sum == 0);
    Tree::clearInit(0, 1);
    Tree one;
    one.modify(0, 1, Tag{2});
    auto snap = one.view();
    one.modify(0, [](const Info &v) { return Info{v.sum * 3}; });
    need(one.query(0, 1).sum == 6 and snap.query(0, 1).sum == 2);
    one.merge(one);
    need(one.query(0, 1).sum == 12 and snap.query(0, 1).sum == 2);
}
struct Plain {
    long long sum = 0;
    Plain operator+(const Plain &v) const { return {sum + v.sum}; }
};
struct Bits {
    unsigned mask = 0;
    void apply(const Bits &v) { mask |= v.mask; }
};
struct Blind {
    unsigned mask = 0;
    Blind operator+(const Blind &v) const { return {mask | v.mask}; }
    void apply(const Bits &t, C, C) { mask |= t.mask; }
};
static void modes() {
    using P = SparseSegTree<Plain, C>;
    static_assert(sizeof(P) == 2 * sizeof(int));
    P::clearInit(-2, 3);
    P p;
    p.modify(0, Plain{2}); p.modify(1, Plain{3});
    need(p.query(-2, 3).sum == 5);
    P q = p;
    q.merge(p, [](const Plain &a, const Plain &b, C) { return Plain{std::max(a.sum,b.sum)}; });
    need(q.query(-2, 3).sum == 5);
    using B = SparseSegTree<Blind, C, Bits>;
    B::clearInit(-8, 8);
    B b;
    b.modify(-8, 8, Bits{1});
    b.modify(-2, 3, Bits{4});
    const auto old = b.view();
    B c;
    c.modify(2, 6, Bits{2});
    b.merge(c);
    need(b.query(-8, -2).mask == 1);
    need(b.query(2, 3).mask == 7);
    need((b + c).query(-8, 8).mask == 7);
    need(old.query(-8, 8).mask == 5);
    need(b.first(-8, 8, [](const Blind &v) { return (v.mask & 4) != 0; }) == -2);
    need(b.last(-8, 8, [](const Blind &v) { return (v.mask & 4) != 0; }) == 2);
}
template <class L>
struct Meta {
    long long sum = 0;
    L len = 1;
    Meta operator+(const Meta &v) const { return {sum + v.sum, len + v.len}; }
    Meta operator-(const Meta &v) const { return {sum - v.sum, len - v.len}; }
    void apply(const ::Tag &t, C l, C r) {
        len = static_cast<L>(r - l);
        sum += t.add * len;
    }
};
template <class V>
static void verifyMeta(const V &v, const Arr &a, C lo) {
    const int n = int(a.size());
    for (int l = 0; l <= n; ++l) {
        for (int r = l; r <= n; ++r) {
            if (l < r) {
                const auto val = v.query(lo + l, lo + r);
                need(val.sum == sum(a, l, r) and val.len == r - l);
            }
            // Length is monotone even when signed roots cancel all payload.
            for (int k = 0; k <= r - l + 1; ++k) {
                std::optional<C> first, last;
                if (l < r and k <= r - l) {
                    first = lo + l + std::max(k, 1) - 1;
                    last = lo + r - std::max(k, 1);
                }
                const auto pred = [k, l, r](const auto &cur) {
                    need(0 < cur.len and cur.len <= r - l);
                    return cur.len >= k;
                };
                need(v.first(lo + l, lo + r, pred) == first);
                need(v.last(lo + l, lo + r, pred) == last);
            }
        }
    }
}
static void metadataCases() {
    using S = SparseSegTree<Meta<int>, C, ::Tag>;
    const C lo = -3;
    S::clearInit(lo, lo + 7, 1);
    S s;
    const Arr zero(7);
    verifyMeta(s, zero, lo); // Info{} may start with nonzero metadata.
    s.modify(lo, lo + 7, ::Tag{1}).modify(-1, 2, ::Tag{3});
    const Arr a{1, 1, 4, 4, 4, 1, 1};
    const auto snap = (s + s) - (s - s);
    Arr doubled = a;
    for (auto &x : doubled) x *= 2;
    verifyMeta(s, a, lo);
    verifyMeta(snap, doubled, lo);
    verifyMeta(s - s, zero, lo);
    S::View<0> empty;
    verifyMeta(empty, zero, lo);
    verifyMeta(empty - (empty - s), a, lo);
    // This distinct tree type starts with cap=1; forks force pool growth.
    for (int i = 0; i < 32; ++i) {
        S branch = s;
        branch.modify(lo + i % 7, Meta<int>{i});
        Arr expected = a;
        expected[i % 7] = i;
        verifyMeta(branch, expected, lo);
    }
    verifyMeta(snap, doubled, lo);
    constexpr C imax = std::numeric_limits<int>::max();
    S::clearInit(-imax, 0);
    S t;
    need(t.query(-imax, 0).len == imax);
    t.modify(-imax, 0, ::Tag{1});
    need((t + t + t + t).query(-imax, 0).len == imax);
    need(((t - t) - (t - t)).query(-imax, 0).sum == 0);

    using W = SparseSegTree<Meta<C>, C, ::Tag>;
    constexpr C low = std::numeric_limits<C>::lowest();
    constexpr C width = std::numeric_limits<C>::max();
    W::clearInit(low, -1);
    W wide;
    wide.modify(low, -1, ::Tag{});
    const auto four = (wide + wide) - (wide - wide);
    need(four.query(low, -1).len == width and four.query(low, -1).sum == 0);
    need(four.query(low, low + 1).len == 1);
    need(four.query(-2, -1).len == 1);
    need(four.first(low, -1, [](const auto &cur) { return cur.len >= width; }) == -2);
    need(four.last(low, -1, [](const auto &cur) { return cur.len >= width; }) == low);
    need(!four.first(low, low, [](const auto &) { return true; }));
    need(!four.last(-1, -1, [](const auto &) { return true; }));
}
// Independent weighted difference Fenwick oracle: integral of a piecewise constant function.
static void largeDomain() {
    constexpr int n = 256;
    constexpr C lim = 1'000'000'000'000'000'000LL;
    std::mt19937_64 rng(testSeed ^ 0x9182A3B4ULL);
    std::vector<std::array<C, 4>> ops(n);
    std::vector<C> xs{-lim, lim};
    for (int i = 0; i < n; ++i) {
        C l = C(rng() % (2 * lim)) - lim;
        C r = i % 2 ? std::min(lim, l + 17) : C(rng() % (2 * lim)) - lim;
        if (l > r) std::swap(l, r);
        C ql = C(rng() % (2 * lim)) - lim;
        C qr = C(rng() % (2 * lim)) - lim;
        if (ql > qr) std::swap(ql, qr);
        if (ql == qr) ++qr;
        ops[i] = {l, r, ql, qr};
        xs.push_back(l);
        xs.push_back(r);
    }
    std::sort(xs.begin(), xs.end());
    xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
    std::vector<I> bit(xs.size() + 1), weighted(xs.size() + 1);
    // libstdc++ debug lower_bound validates whole partitions; keep the oracle O(log n).
    const auto pos = [&](C x, bool upper) {
        std::size_t l = 0, r = xs.size();
        while (l < r) {
            const auto m = l + (r - l) / 2;
            if (xs[m] < x or (upper and xs[m] == x)) l = m + 1;
            else r = m;
        }
        return l;
    };
    const auto add = [&](C x, I d) {
        for (std::size_t i = pos(x, false) + 1;
             i < bit.size(); i += i & -i) {
            bit[i] += d;
            weighted[i] += d * x;
        }
    };
    const auto prefix = [&](C x) {
        I a = 0, b = 0;
        for (std::size_t i = pos(x, true);
             i; i -= i & -i) {
            a += bit[i];
            b += weighted[i];
        }
        return I(x) * a - b;
    };
    Tree::clearInit(-lim, lim, 1); // Tiny reserve exercises repeated vector reallocations.
    Tree t;
    t.modify(-lim, lim, Tag{3});
    add(-lim, 3);
    add(lim, -3);
    auto root = t.view();
    for (int i = 0; i < n; ++i) {
        const auto &op = ops[i];
        const I d = i % 9 - 4;
        t.modify(op[0], op[1], Tag{d});
        add(op[0], d);
        add(op[1], -d);
        need(t.query(op[2], op[3]).sum == prefix(op[3]) - prefix(op[2]));
        need(root.query(op[2], op[3]).sum == I(3) * (op[3] - op[2]));
    }
    const auto snap = t.view();
    const I total = prefix(lim) - prefix(-lim);
    t.merge(t);
    need(t.query(-lim, lim).sum == 2 * total);
    need(snap.query(-lim, lim).sum == total);
    for (const auto &op : ops)
        need(t.query(op[2], op[3]).sum == 2 * (prefix(op[3]) - prefix(op[2])));
}
} // namespace lazy_test

static void footerExample() {
    const auto joined = Info{5, 2} + Info{7, 3};
    CHECK(joined.sum == 12 and joined.len == 5);
    using S = SparseSegTree<Info, long long, Tag>;
    S::clearInit(-100, 100, 1);
    S s;
    CHECK(s.query(-7, 23).sum == 0 and s.query(-7, 23).len == 30);
    s.modify(10, 20, Tag{1});
    s.modify(15, 25, Tag{1});
    CHECK(s.query(10, 25).sum == 20 and s.query(10, 25).len == 15);
    auto old = s;
    s.modify(-100, 100, Tag{2});
    s.modify(18, [value = std::make_unique<long long>(9)](const Info &v) {
        CHECK(v.len == 1 and v.sum == 4);
        return Info{*value};
    });
    CHECK(old.query(10, 25).sum == 20);
    CHECK(s.query(10, 25).sum == 55);
    auto diff = s - old;
    CHECK(diff.query(-100, 100).len == 200 and diff.query(-100, 100).sum == 405);
    auto zero = (s + old) - (s + old);
    CHECK(zero.query(10, 25).sum == 0 and zero.query(10, 25).len == 15);
    s.merge(old);
    CHECK(s.query(10, 25).sum == 75 and s.query(10, 25).len == 15);
    // INT_MAX length remains safe even when combining four roots.
    constexpr long long n = std::numeric_limits<int>::max();
    S::clearInit(-n, 0);
    S a;
    a.modify(-n, 0, Tag{1});
    auto b = a;
    auto four = a + b + a + b;
    CHECK(four.query(-n, 0).sum == 4 * n and four.query(-n, 0).len == n);
    auto signedFour = (a + b) - (a + b);
    CHECK(signedFour.query(-n, 0).sum == 0 and signedFour.query(-n, 0).len == n);
    a.modify(-1, Info{7});
    CHECK(a.query(-n, 0).sum == n + 6 and b.query(-n, 0).sum == n);
    CHECK(a.last(-n, 0, [](const Info &v) { return v.sum >= 7; }) == -1);
}

int main() {
    runCase("SparseSegTree/lazy-exhaustive", lazy_test::exhaustive);
    runCase("SparseSegTree/lazy-random-cow-merge-views", lazy_test::randomMixed);
    runCase("SparseSegTree/lazy-prefix-suffix-search", lazy_test::searchCases);
    runCase("SparseSegTree/lazy-nested-view-search", lazy_test::nestedViews);
    runCase("SparseSegTree/lazy-metadata-length", lazy_test::metadataCases);
    runCase("SparseSegTree/lazy-extreme-coordinates", lazy_test::extremes);
    runCase("SparseSegTree/lazy-type-modes", lazy_test::modes);
    runCase("SparseSegTree/lazy-large-domain", lazy_test::largeDomain);
    runCase("SparseSegTree/lazy-footer-int-length", footerExample);
    std::cout << "lazy assertions=" << lazy_test::checks << '\n';
    return 0;
}
