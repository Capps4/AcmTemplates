#include "../../../../../src/core/DataStructures/SparseSegTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace contract_test {
struct Ordered {
    std::string text{};
    Ordered operator+(const Ordered &v) const {
        return {text + v.text};
    }
};

template <class V>
static void verifyOrdered(const V &tree, const std::array<std::string, 7> &a) {
    constexpr long long lo = -3;
    for (int l = 0; l <= 7; ++l) {
        for (int r = l; r <= 7; ++r) {
            std::string joined;
            for (int x = l; x < r; ++x)
                joined += a[x];
            if (l < r)
                CHECK(tree.query(lo + l, lo + r).text == joined);
            for (const std::string pattern : {"bc", "ab", "ca", "zz"}) {
                std::optional<long long> first, last;
                std::string prefix, suffix;
                for (int x = l; x < r; ++x) {
                    prefix += a[x];
                    if (prefix.find(pattern) != std::string::npos) {
                        first = lo + x;
                        break;
                    }
                }
                for (int x = r - 1; x >= l; --x) {
                    suffix = a[x] + suffix;
                    if (suffix.find(pattern) != std::string::npos) {
                        last = lo + x;
                        break;
                    }
                }
                const auto pred = [&](const Ordered &v) {
                    return v.text.find(pattern) != std::string::npos;
                };
                CHECK(tree.first(lo + l, lo + r, pred) == first);
                CHECK(tree.last(lo + l, lo + r, pred) == last);
            }
        }
    }
}

static void plainOrdered() {
    using Tree = SparseSegTree<Ordered, long long>;
    Tree::clearInit(-3, 4, 1);
    Tree a, b;
    const std::array<std::string, 7> left{"a", "b", "c", "a", "b", "c", "z"};
    const std::array<std::string, 7> right{"x", "", "y", "", "z", "q", ""};
    for (int i = 0; i < 7; ++i) {
        a.modify(-3 + i, Ordered{left[i]});
        b.modify(-3 + i, Ordered{right[i]});
    }
    verifyOrdered(a, left);
    const auto old = a.view();
    int calls = 0;
    a.merge(b, [&](const Ordered &x, const Ordered &y, long long pos) {
        CHECK(-3 <= pos and pos < 4);
        CHECK(x.text == left[pos + 3] and y.text == right[pos + 3]);
        ++calls;
        return Ordered{x.text + y.text};
    });
    CHECK(calls == 7);
    auto joined = left;
    for (int i = 0; i < 7; ++i)
        joined[i] += right[i];
    verifyOrdered(a, joined);
    verifyOrdered(b, right);
    verifyOrdered(old, left);
}

template <class Coord>
struct CoordinateInfo {
    long long sum = 0;
    Coord l{}, r{};
    long long len = 0;
    CoordinateInfo operator+(const CoordinateInfo &v) const {
        return {sum + v.sum};
    }
    CoordinateInfo operator-(const CoordinateInfo &v) const {
        return {sum - v.sum};
    }
    void apply(const ::Tag &t, Coord a, Coord b) {
        l = a;
        r = b;
        len = static_cast<long long>(b - a);
        sum += t.add * len;
    }
};

template <class Coord>
static void coordinateDomain(Coord lo) {
    constexpr int n = 5;
    using Value = CoordinateInfo<Coord>;
    using Tree = SparseSegTree<Value, Coord, ::Tag>;
    const auto pos = [&](int x) { return Coord(lo + Coord(x)); };
    Tree::clearInit(lo, pos(n), 1);
    Tree tree;
    std::array<long long, n> a{2, 5, 9, 5, 3};
    tree.modify(lo, pos(n), ::Tag{2});
    tree.modify(pos(1), pos(4), ::Tag{3});
    tree.modify(pos(2), Value{9});
    tree.modify(pos(4), [&](const Value &v) {
        CHECK(v.sum == 2 and v.l == pos(4) and v.r == pos(5) and v.len == 1);
        return Value{v.sum + 1};
    });
    for (int l = 0; l <= n; ++l) {
        for (int r = l; r <= n; ++r) {
            long long sum = 0;
            for (int x = l; x < r; ++x)
                sum += a[x];
            if (l < r) {
                const auto got = tree.query(pos(l), pos(r));
                CHECK(got.sum == sum and got.len == r - l);
                CHECK(got.l == pos(l) and got.r == pos(r));
            }
            for (long long k = 0; k <= sum + 1; ++k) {
                std::optional<Coord> first, last;
                long long acc = 0;
                for (int x = l; x < r; ++x) {
                    acc += a[x];
                    if (acc >= k) {
                        first = pos(x);
                        break;
                    }
                }
                acc = 0;
                for (int x = r - 1; x >= l; --x) {
                    acc += a[x];
                    if (acc >= k) {
                        last = pos(x);
                        break;
                    }
                }
                CHECK(tree.first(pos(l), pos(r), [&](const Value &v) {
                    CHECK(v.l == pos(l) and v.r <= pos(r) and v.len > 0);
                    return v.sum >= k;
                }) == first);
                CHECK(tree.last(pos(l), pos(r), [&](const Value &v) {
                    CHECK(v.l >= pos(l) and v.r == pos(r) and v.len > 0);
                    return v.sum >= k;
                }) == last);
            }
        }
    }
}

static void coordinateTypes() {
    coordinateDomain(std::numeric_limits<int>::min());
    coordinateDomain(std::numeric_limits<int>::max() - 5);
    coordinateDomain(std::numeric_limits<unsigned long long>::max() - 5);
}

using Owned = SparseSegTree<::Info, long long, ::Tag>;
using Values = std::array<long long, 5>;

template <class V>
static void verifyValues(const V &tree, const Values &a, long long lo = -2) {
    for (int l = 0; l < 5; ++l) {
        long long sum = 0;
        for (int r = l + 1; r <= 5; ++r) {
            sum += a[r - 1];
            const auto got = tree.query(lo + l, lo + r);
            CHECK(got.sum == sum and got.len == r - l);
        }
    }
}

static void add(Owned &tree, Values &a, int l, int r, long long delta) {
    tree.modify(-2 + l, -2 + r, ::Tag{delta});
    for (int x = l; x < r; ++x)
        a[x] += delta;
}

static void lifecycle() {
    Owned::clearInit(-2, 3, 1);
    {
        const Values zero{};
        Owned a, b;
        Values av{}, bv{};
        add(a, av, 0, 5, 1);
        add(a, av, 0, 3, 2);
        a.modify(-1, ::Info{8});
        av[1] = 8;
        add(b, bv, 0, 5, 3);
        add(b, bv, 2, 5, 4);
        b.modify(2, [&](const ::Info &v) {
            CHECK(v.sum == 7 and v.len == 1);
            return ::Info{v.sum + 2};
        });
        bv[4] += 2;
        // No copy or View has frozen either input: this must exercise join<InPlace>.
        a.merge(std::move(b));
        for (int x = 0; x < 5; ++x)
            av[x] += bv[x];
        bv = zero;
        verifyValues(a, av);
        verifyValues(b, bv);
        add(a, av, 0, 5, 2);
        add(b, bv, 1, 4, 5);
        verifyValues(a, av);
        verifyValues(b, bv);

        Owned imported;
        imported.merge(std::move(b));
        verifyValues(imported, bv);
        verifyValues(b, zero);
        bv = zero;
        add(b, bv, 0, 5, 1);
        a.merge(Owned{});
        verifyValues(a, av);
        verifyValues(b, bv);

        const auto aOld = a.view();
        const Values aSaved = av;
        Owned copy = a;
        Values cv = av;
        add(a, av, 0, 2, 3);
        copy.modify(2, ::Info{17});
        cv[4] = 17;
        verifyValues(a, av);
        verifyValues(copy, cv);
        verifyValues(aOld, aSaved);

        Owned assigned;
        Values dv{};
        add(assigned, dv, 0, 5, 7);
        const auto destinationOld = assigned.view();
        const Values destinationSaved = dv;
        const auto copyOld = copy.view();
        const Values copySaved = cv;
        assigned = copy;
        dv = cv;
        add(assigned, dv, 1, 4, 2);
        Owned *alias = &assigned;
        assigned = *alias;
        assigned = std::move(*alias);
        verifyValues(assigned, dv);
        verifyValues(copy, cv);
        verifyValues(destinationOld, destinationSaved);
        verifyValues(copyOld, copySaved);

        const auto sourceOld = assigned.view();
        const Values sourceSaved = dv;
        Owned moved(std::move(assigned));
        Values mv = dv;
        verifyValues(assigned, zero);
        verifyValues(moved, mv);
        dv = zero;
        add(assigned, dv, 0, 5, 3);
        verifyValues(assigned, dv);
        verifyValues(sourceOld, sourceSaved);

        Owned target;
        Values tv{};
        add(target, tv, 0, 5, 9);
        const auto targetOld = target.view();
        const Values targetSaved = tv;
        const auto movedOld = moved.view();
        const Values movedSaved = mv;
        target = std::move(moved);
        tv = mv;
        verifyValues(target, tv);
        verifyValues(moved, zero);
        target.modify(-2, ::Info{5});
        tv[0] = 5;
        mv = zero;
        add(moved, mv, 0, 5, 4);
        verifyValues(target, tv);
        verifyValues(moved, mv);
        verifyValues(targetOld, targetSaved);
        verifyValues(movedOld, movedSaved);
        verifyValues(sourceOld, sourceSaved);

        Owned shared = target;
        const auto sharedOld = target.view();
        const Values sharedSaved = tv;
        target.merge(std::move(shared));
        for (auto &v : tv)
            v *= 2;
        verifyValues(target, tv);
        verifyValues(shared, zero);
        verifyValues(sharedOld, sharedSaved);
        const auto beforeSelf = target.view();
        const Values selfSaved = tv;
        target.merge(target);
        for (auto &v : tv)
            v *= 2;
        verifyValues(target, tv);
        verifyValues(beforeSelf, selfSaved);

        const auto preserved = target.view();
        const Values preservedValues = tv;
        target = Owned{};
        tv = zero;
        verifyValues(target, tv);
        add(target, tv, 0, 5, 2);
        verifyValues(target, tv);
        verifyValues(preserved, preservedValues);
        const auto beforeEmptyAssignment = target.view();
        const Values emptyAssignmentSaved = tv;
        Owned empty;
        Owned emptyCopy(empty);
        target = emptyCopy;
        verifyValues(target, zero);
        verifyValues(emptyCopy, zero);
        verifyValues(beforeEmptyAssignment, emptyAssignmentSaved);
        tv = zero;
        add(target, tv, 0, 5, 1);
        verifyValues(target, tv);
        verifyValues(empty, zero);
        const auto detached = [] {
            Owned local;
            local.modify(-2, 3, ::Tag{6});
            return local.view();
        }();
        verifyValues(detached, Values{6, 6, 6, 6, 6});
    }
    // Reinitialization invalidates old roots; only freshly constructed trees are inspected.
    Owned::clearInit(7, 12, 1);
    Owned fresh;
    verifyValues(fresh, Values{}, 7);
    fresh.modify(7, 12, ::Tag{4});
    verifyValues(fresh, Values{4, 4, 4, 4, 4}, 7);
}
} // namespace contract_test

int main() {
    runCase("SparseSegTree/plain-ordered-info", contract_test::plainOrdered);
    runCase("SparseSegTree/coordinate-types", contract_test::coordinateTypes);
    runCase("SparseSegTree/lazy-ownership-lifecycle", contract_test::lifecycle);
    return 0;
}
