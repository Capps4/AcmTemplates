#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/DataStructures/TreeDataStructures/SegTree/code.hpp"
#include "TestTypes.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>
#include <numeric>
#include <utility>


template <class Tree, class U, class = void>
struct HasUpdate : std::false_type {};
template <class Tree, class U>
struct HasUpdate<Tree, U, std::void_t<decltype(std::declval<Tree &>().modify(
    0, 0, std::declval<const U &>()))>> : std::true_type {};

template <class Tree, class U, class = void>
struct HasExplicitUpdate : std::false_type {};
template <class Tree, class U>
struct HasExplicitUpdate<Tree, U, std::void_t<decltype(
    std::declval<Tree &>().template modify<U>(0, 0, std::declval<const U &>()))>>
    : std::true_type {};

static_assert(HasUpdate<SegTree<SumInfo, AddTag>, AddTag>::value);
static_assert(!HasUpdate<SegTree<SumInfo>, AddTag>::value);
static_assert(!HasExplicitUpdate<SegTree<SumInfo>, AddTag>::value);
static_assert(!HasExplicitUpdate<SegTree<SumInfo, AddTag>, AffineTag>::value);
static_assert(std::is_empty_v<_segt::Tags<void>>);
static_assert(!std::is_default_constructible_v<SumInfo>);

struct NoCopyPred {
    std::unique_ptr<long long> lim;
    bool operator()(const MaxInfo &v) { return v.val >= *lim; }
};

int trialId = 0, stepId = 0, caseN = 0, caseL = 0, caseR = 0;
void context() {
    std::cerr << "trial=" << trialId << " step=" << stepId << " n=" << caseN
              << " l=" << caseL << " r=" << caseR << '\n';
}
void expect(bool ok) {
    if (!ok) context();
    CHECK(ok);
}

template <class Tree>
void verifySum(Tree &tree, const std::vector<long long> &a) {
    int n = int(a.size());
    if (!n) return;
    expect(tree.query(0, n).val == std::accumulate(a.begin(), a.end(), 0LL));
    expect(tree.query(0, n).len == n);
    for (int l = 0; l <= n; ++l) {
        for (int r = l + 1; r <= n; ++r) {
            caseL = l; caseR = r;
            auto v = tree.query(l, r);
            expect(v.val == std::accumulate(a.begin() + l, a.begin() + r, 0LL));
            expect(v.len == r - l);
        }
    }
    for (int i = 0; i < n; ++i) {
        expect(tree.query(i, i + 1).val == a[i]);
        expect(tree.query(i, i + 1).len == 1);
    }
}

void boundaries() {
    for (int n = 0; n <= 17; ++n) {
        caseN = n;
        SegTree<SumInfo> plain(n, SumInfo(3));
        SegTree<SumInfo, AddTag> lazy(n, SumInfo(3));
        lazy.modify(0, n, AddTag{2});
        lazy.modify(n, n, AddTag{100});
        verifySum(plain, std::vector<long long>(n, 3));
        verifySum(lazy, std::vector<long long>(n, 5));
        int calls = 0;
        auto pred = [&calls](const SumInfo &) { ++calls; return true; };
        expect(lazy.first(0, 0, pred) == std::nullopt);
        expect(lazy.last(n, n, pred) == std::nullopt);
        expect(calls == 0);
        if (n) {
            auto first = lazy.first(0, n, pred), last = lazy.last(0, n, pred);
            expect(first.has_value() && *first == 0);
            expect(last.has_value() && *last == n - 1);
            auto never = [](const SumInfo &) { return false; };
            expect(!lazy.first(0, n, never) && !lazy.last(0, n, never));
        }
        auto copy = lazy;
        if (n) { copy.modify(n - 1, SumInfo(9)); expect(lazy.query(n - 1, n - 1 + 1).val == 5); }
        for (int l = 0; l <= n; ++l) {
            for (int r = l; r <= n; ++r) {
                std::vector<long long> a(n);
                std::iota(a.begin(), a.end(), -5LL);
                SegTree<SumInfo, AffineTag> tree(a);
                auto update = [&](int ql, int qr, AffineTag tag) {
                    tree.modify(ql, qr, tag);
                    for (int i = ql; i < qr; ++i) a[i] = a[i] * tag.mul + tag.add;
                };
                update(0, n, {0, 4});
                update(0, n, {-1, 3});
                update(l, r, {0, 7});
                update(0, n, {-1, 1});
                update(l, r, {1, -2});
                auto pending = tree;
                if (n) {
                    int p = (l + r) / 2 % n;
                    tree.modify(p, SumInfo(-11));
                    a[p] = -11;
                    expect(pending.query(p, p + 1).val != -11);
                }
                verifySum(tree, a);
            }
        }
    }
    SegTree<SumInfo> bits(std::vector<bool>{true, false, true});
    verifySum(bits, std::vector<long long>{1, 0, 1});
}

void exhaustiveSearch() {
    for (int n = 0; n <= 5; ++n) {
        caseN = n;
        for (int mask = 0; mask < (1 << n); ++mask) {
            trialId = mask;
            std::vector<long long> a(n);
            for (int i = 0; i < n; ++i) a[i] = (mask >> i) & 1;
            SegTree<MaxInfo> plain(a);
            SegTree<MaxInfo, AddTag> lazy(a);
            lazy.modify(0, n, AddTag{3});
            for (int l = 0; l <= n; ++l) {
                for (int r = l; r <= n; ++r) {
                    caseL = l; caseR = r;
                    for (int lim = 0; lim <= 2; ++lim) {
                        std::optional<int> first, last;
                        for (int i = l; i < r; ++i) {
                            if (a[i] >= lim) { if (!first) first = i; last = i; }
                        }
                        auto pred = [lim](const MaxInfo &v) { return v.val >= lim; };
                        auto shifted = [lim](const MaxInfo &v) { return v.val >= lim + 3; };
                        expect(plain.first(l, r, pred) == first);
                        expect(plain.last(l, r, pred) == last);
                        expect(lazy.first(l, r, shifted) == first);
                        expect(lazy.last(l, r, shifted) == last);
                    }
                }
            }
        }
    }
    std::vector<long long> a{1, 1, 1, 1, 5, 1, 1, 1};
    SegTree<SumInfo, AddTag> tree(a);
    for (int lim = 0; lim <= 20; ++lim) {
        for (int l = 0; l <= 8; ++l) {
            for (int r = l; r <= 8; ++r) {
                std::optional<int> first, last;
                for (int i = l; i < r; ++i) {
                    if (a[i] >= lim) { if (!first) first = i; last = i; }
                }
                auto pred = [lim](const SumInfo &v) { return v.val >= lim; };
                expect(tree.first(l, r, pred) == first);
                expect(tree.last(l, r, pred) == last);
            }
        }
    }
}

void randomUpdates() {
    for (int trial = 0; trial < 8; ++trial) {
        test_context::step = trial;
        trialId = trial;
        int n = trial < 20 ? trial : randomInt(1, 24);
        caseN = n;
        std::vector<long long> a(n);
        for (auto &v : a) v = randomInt(-100, 100);
        SegTree<SumInfo> plain(a);
        SegTree<SumInfo, AffineTag> affine(a);
        SegTree<SumInfo, AddTag> sums(a);
        SegTree<MaxInfo, AddTag> maxima(a);
        std::vector<long long> b = a, c = a;
        for (int step = 0; step < 32; ++step) {
        test_context::step = step;
            stepId = step;
            int l = randomInt(0, n), r = randomInt(l, n);
            caseL = l; caseR = r;
            if (n && step % 4 == 0) {
                int p = randomInt(0, n - 1), v = randomInt(-200, 200);
                a[p] = b[p] = c[p] = v;
                plain.modify(p, SumInfo(v)); affine.modify(p, SumInfo(v));
                sums.modify(p, SumInfo(v)); maxima.modify(p, MaxInfo(v));
            } else {
                AddTag tag{randomInt(-20, 20)};
                AffineTag f{randomInt(-1, 1), randomInt(-5, 5)};
                sums.modify(l, r, tag); maxima.modify(l, r, tag);
                affine.modify(l, r, f);
                for (int i = l; i < r; ++i) { a[i] += tag.add; b[i] = b[i] * f.mul + f.add; }
            }
            if (l < r) {
                expect(plain.query(l, r).val == std::accumulate(c.begin() + l, c.begin() + r, 0LL));
                expect(sums.query(l, r).val == std::accumulate(a.begin() + l, a.begin() + r, 0LL));
                expect(affine.query(l, r).val == std::accumulate(b.begin() + l, b.begin() + r, 0LL));
                expect(sums.query(l, r).len == r - l);
            }
            if (n) {
                expect(affine.query(0, n).val == std::accumulate(b.begin(), b.end(), 0LL));
                expect(sums.query(0, n).val == std::accumulate(a.begin(), a.end(), 0LL));
                expect(sums.query(0, n).len == n && maxima.query(0, n).len == n);
            }
            long long mx = l == r ? std::numeric_limits<long long>::lowest()
                                 : *std::max_element(a.begin() + l, a.begin() + r);
            if (l < r) expect(maxima.query(l, r).val == mx);
            int lim = randomInt(-250, 250);
            std::optional<int> first, last;
            for (int i = l; i < r; ++i) {
                if (a[i] >= lim) { if (!first) first = i; last = i; }
            }
            expect(maxima.first(l, r, NoCopyPred{std::make_unique<long long>(lim)}) == first);
            expect(maxima.last(l, r, NoCopyPred{std::make_unique<long long>(lim)}) == last);
            if (n && step % 7 == 0) {
                int p = randomInt(0, n - 1);
                expect(sums.query(p, p + 1).val == a[p] && affine.query(p, p + 1).val == b[p]);
                expect(maxima.query(p, p + 1).val == a[p] && plain.query(p, p + 1).val == c[p]);
            }
        }
    }
}

void orderedMerge() {
    for (int n = 1; n <= 17; ++n) {
        caseN = n;
        std::vector<char> a(n);
        for (int i = 0; i < n; ++i) a[i] = char('a' + i % 26);
        SegTree<TextInfo> tree(a);
        for (int l = 0; l < n; ++l) {
            for (int r = l + 1; r <= n; ++r) {
                caseL = l; caseR = r;
                expect(tree.query(l, r).val == std::string(a.begin() + l, a.begin() + r));
            }
        }
        expect(tree.query(0, n).val == std::string(a.begin(), a.end()));
    }
    for (int trial = 0; trial < 4; ++trial) {
        test_context::step = trial;
        trialId = trial;
        int n = randomInt(0, 40);
        caseN = n;
        std::vector<char> a(n, 'a'), b = a;
        SegTree<TextInfo> plain(a);
        SegTree<SetInfo, SetTag> lazy(a);
        for (int step = 0; step < 16; ++step) {
        test_context::step = step;
            stepId = step;
            int l = randomInt(0, n), r = randomInt(l, n);
            caseL = l; caseR = r;
            char c = char('a' + randomInt(0, 3));
            lazy.modify(l, r, SetTag{c, true});
            std::fill(b.begin() + l, b.begin() + r, c);
            if (n) {
                int p = randomInt(0, n - 1);
                a[p] = b[p] = c;
                plain.modify(p, TextInfo(c)); lazy.modify(p, SetInfo(c));
            }
            if (l < r) {
                expect(plain.query(l, r).val == std::string(a.begin() + l, a.begin() + r));
                expect(lazy.query(l, r).val == std::string(b.begin() + l, b.begin() + r));
            }
            if (n) expect(lazy.query(0, n).val == std::string(b.begin(), b.end()));
            auto pred = [](const SetInfo &v) { return v.val.find('c') != std::string::npos; };
            std::optional<int> first, last;
            for (int i = l; i < r; ++i) {
                if (b[i] == 'c') { if (!first) first = i; last = i; }
            }
            expect(lazy.first(l, r, pred) == first);
            expect(lazy.last(l, r, pred) == last);
        }
    }
}

void largeSearch() {
    constexpr int n = 129;
    SegTree<MaxInfo, AddTag> tree(n, MaxInfo(0));
    tree.modify(0, n, AddTag{3});
    tree.modify(n - 1, n, AddTag{4});
    int calls = 0;
    auto pred = [&calls](const MaxInfo &v) { ++calls; return v.val >= 7; };
    expect(tree.first(0, n, pred) == n - 1);
    expect(calls < 100);
    calls = 0;
    expect(tree.last(0, n, pred) == n - 1);
    expect(calls < 100);
    tree.modify(0, MaxInfo(8));
    expect(tree.first(0, n, pred) == 0);
    expect(tree.last(0, n - 1, pred) == 0);
}



#include "../../../../../src/DataStructures/TreeDataStructures/SegTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
#include "TestTypes.hpp"

namespace boundary_cases {
void verifyAdded(int n, int shape) {
    std::vector<long long> a(n);
    std::iota(a.begin(), a.end(), 1);
    auto d = SegTree<SumInfo, AffineTag>(a);
    auto verifyState = [&] {
        for (int l = 0; l < n; ++l)
            for (int r = l + 1; r <= n; ++r)
                CHECK(d.query(l, r).val == std::accumulate(a.begin() + l, a.begin() + r, 0LL));
    };
    verifyState();
    for (int step = 0; step < 12 and n > 0; ++step) {
        test_context::step = step;
        int l = shape ? 0 : step % n, r = shape ? n : std::min(n, l + 1 + step % 4);
        AffineTag t{step % 3 == 0 ? -1 : 1, step - 5};
        d.modify(l, r, t);
        for (int i = l; i < r; ++i)
            a[i] = a[i] * t.mul + t.add;
        auto saved = d;
        verifyState();
        for (int i = 0; i < n; ++i)
            CHECK(saved.query(i, i + 1).val == a[i]);
        int p = step % n;
        d.modify(p, SumInfo(step));
        a[p] = step;
        verifyState();
    }
}

int run() {
    runCase("SegTree/empty", [] {
        verifyAdded(0, 0);
    });
    runCase("SegTree/single", [] {
        verifyAdded(1, 0);
    });
    runCase("SegTree/two-elements-alternate-tags", [] {
        verifyAdded(2, 1);
    });
    runCase("SegTree/odd-padding", [] {
        verifyAdded(3, 0);
    });
    runCase("SegTree/power-of-two", [] {
        verifyAdded(4, 1);
    });
    runCase("SegTree/non-power-of-two", [] {
        verifyAdded(5, 0);
    });
    runCase("SegTree/padding-search", [] {
        verifyAdded(7, 1);
    });
    runCase("SegTree/range-composition", [] {
        verifyAdded(17, 0);
    });
    return 0;
}
}

int main() {
    runCase("SegTree/boundaries", [] { boundaries(); });
    runCase("SegTree/exhaustiveSearch", [] { exhaustiveSearch(); });
    runCase("SegTree/randomUpdates", [] { randomUpdates(); });
    runCase("SegTree/orderedMerge", [] { orderedMerge(); });
    runCase("SegTree/largeSearch", [] { largeSearch(); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
