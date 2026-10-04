#include "Final.hpp"
#include "../TestSupport.hpp"
#include <algorithm>
#include <limits>
#include <memory>
#include <numeric>

struct MaxInfo {
    long long val;
    explicit MaxInfo(long long val = std::numeric_limits<long long>::lowest()) : val(val) {}
    void apply(const Tag& tag) { val += tag.add; }
    MaxInfo operator+(const MaxInfo& other) const { return MaxInfo(std::max(val, other.val)); }
};
struct AffineTag {
    long long mul = 1, add = 0;
    void apply(const AffineTag& after) { mul *= after.mul; add = add * after.mul + after.add; }
};
struct SumInfo {
    long long val = 0;
    int len = 1;
    explicit SumInfo(long long val = 0, int len = 1) : val(val), len(len) {}
    void apply(const AffineTag& tag) { val = val * tag.mul + tag.add * len; }
    SumInfo operator+(const SumInfo& other) const { return SumInfo(val + other.val, len + other.len); }
};
struct NoCopyPredicate {
    std::unique_ptr<long long> threshold;
    bool operator()(const MaxInfo& info) const { return info.val >= *threshold; }
};
int main() {
    LazySegT<Node, Tag> empty(std::vector<int>{});
    empty.rangeModify(0, 0, Tag(1));
    CHECK(empty.findFirst(0, 0, [](const Node&) { return true; }) == -1);
    for (int trial = 0; trial < 120; ++trial) {
        int n = randomInt(1, 100);
        std::vector<long long> a(n);
        for (auto& x : a) x = randomInt(-100, 100);
        LazySegT<Node, Tag> sums(a);
        LazySegT<MaxInfo, Tag> maxima(a);
        for (int step = 0; step < 400; ++step) {
            int l = randomInt(0, n), r = randomInt(l, n);
            if (randomInt(0, 2) == 0) {
                int delta = randomInt(-20, 20);
                sums.rangeModify(l, r, Tag(delta)); maxima.rangeModify(l, r, Tag(delta));
                for (int i = l; i < r; ++i) a[i] += delta;
            } else if (randomInt(0, 2) == 0) {
                int i = randomInt(0, n - 1), value = randomInt(-200, 200);
                a[i] = value; sums.modify(i, Node(value)); maxima.modify(i, MaxInfo(value));
            }
            if (l != r) {
                CHECK(sums.rangeQuery(l, r).val == std::accumulate(a.begin() + l, a.begin() + r, 0LL));
                CHECK(sums.rangeQuery(l, r).len == r - l);
                CHECK(maxima.rangeQuery(l, r).val == *std::max_element(a.begin() + l, a.begin() + r));
            }
            long long threshold = randomInt(-200, 200);
            int first = -1, last = -1;
            for (int i = l; i < r; ++i) if (a[i] >= threshold) { if (first == -1) first = i; last = i; }
            CHECK(maxima.findFirst(l, r, NoCopyPredicate{std::make_unique<long long>(threshold)}) == first);
            CHECK(maxima.findLast(l, r, NoCopyPredicate{std::make_unique<long long>(threshold)}) == last);
        }
    }
    for (int trial = 0; trial < 100; ++trial) {
        int n = randomInt(1, 50);
        std::vector<long long> a(n, 0);
        LazySegT<SumInfo, AffineTag> tree(a);
        for (int step = 0; step < 300; ++step) {
            int l = randomInt(0, n), r = randomInt(l, n);
            AffineTag tag{randomInt(-1, 1), randomInt(-5, 5)};
            tree.rangeModify(l, r, tag);
            for (int i = l; i < r; ++i) a[i] = a[i] * tag.mul + tag.add;
            int ql = randomInt(0, n - 1), qr = randomInt(ql + 1, n);
            CHECK(tree.rangeQuery(ql, qr).val == std::accumulate(a.begin() + ql, a.begin() + qr, 0LL));
        }
    }
    std::cout << "Random range/point updates, max searches, move-only predicates and affine composition passed\n";
}
