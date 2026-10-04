#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <utility>
#include <vector>

namespace legacy {
#include "Original.hpp"
}

struct BenchTag {
    long long add = 0;
    explicit BenchTag(long long add = 0) : add(add) {}
    void apply(const BenchTag &v) { add += v.add; }
};
struct BenchInfo {
    long long val;
    int len;
    explicit BenchInfo(long long val = 0, int len = 1) : val(val), len(len) {}
    BenchInfo operator+(const BenchInfo &v) const { return BenchInfo(val + v.val, len + v.len); }
    void apply(const BenchTag &v) { val += v.add * len; }
};
struct BenchMax {
    long long val;
    explicit BenchMax(long long val = std::numeric_limits<long long>::lowest()) : val(val) {}
    void apply(const BenchTag &tag) { val += tag.add; }
    BenchMax operator+(const BenchMax &v) const { return BenchMax(std::max(val, v.val)); }
};
template <class T>
struct RefTree : legacy::LazySegT<T, BenchTag> {
    using Base = legacy::LazySegT<T, BenchTag>;
    using Base::Base;

    void modify(int p, const T &v) { Base::modify(p, v); }
    void modify(int l, int r, const BenchTag &v) { Base::rangeModify(l, r, v); }
    T query(int l, int r) { return Base::rangeQuery(l, r); }
    template <class F>
    std::optional<int> first(int l, int r, F pred) {
        int p = Base::findFirst(l, r, std::move(pred));
        if (p == -1) return std::nullopt;
        return p;
    }
    template <class F>
    std::optional<int> last(int l, int r, F pred) {
        int p = Base::findLast(l, r, std::move(pred));
        if (p == -1) return std::nullopt;
        return p;
    }
};

struct Op { int l, r, v; };

int main() {
    constexpr int n = 100000, q = 100000;
    std::vector<int> a(n, 1);
    std::mt19937 rng(20261004);
    std::vector<Op> ops(q);
    for (auto &[l, r, v] : ops) {
        l = int(rng() % n); r = l + 1 + int(rng() % (n - l)); v = int(rng() % 10);
    }
    compare("lazy-build-n100K-x20", [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20; ++i) { RefTree<BenchInfo> tree(a); sum += tree.query(0, n).val; }
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20; ++i) { SegTree<BenchInfo, BenchTag> tree(a); sum += tree.query(0, n).val; }
        return sum;
    });
    auto ranges = [&](auto &tree) {
        std::uint64_t sum = 0;
        for (auto [l, r, v] : ops) { tree.modify(l, r, BenchTag(v)); sum += tree.query(l, r).val; }
        return sum;
    };
    compare("lazy-range-update-query-n100K-q100K", [&] {
        RefTree<BenchInfo> tree(a); return ranges(tree);
    }, [&] {
        SegTree<BenchInfo, BenchTag> tree(a); return ranges(tree);
    });
    auto points = [&](auto &tree) {
        std::uint64_t sum = 0;
        for (auto [l, r, v] : ops) { tree.modify(l, BenchInfo(v)); sum += tree.query(l, r).val; }
        return sum;
    };
    compare("plain-point-query-vs-recursive-lazy-n100K-q100K", [&] {
        RefTree<BenchInfo> tree(a); return points(tree);
    }, [&] {
        SegTree<BenchInfo> tree(a); return points(tree);
    });
    std::vector<int> values(n);
    for (auto &v : values) v = int(rng() % 1000);
    auto searches = [&](auto &tree) {
        std::uint64_t sum = 0;
        tree.modify(0, n, BenchTag(1));
        for (auto [l, r, v] : ops) {
            auto pred = [lim = 990 + v](const BenchMax &info) { return info.val >= lim; };
            sum += std::uint64_t(tree.first(l, r, pred).value_or(-1) + 1);
            sum += std::uint64_t(tree.last(l, r, pred).value_or(-1) + 1);
        }
        return sum;
    };
    compare("lazy-max-search-n100K-q100K", [&] {
        RefTree<BenchMax> tree(values); return searches(tree);
    }, [&] {
        SegTree<BenchMax, BenchTag> tree(values); return searches(tree);
    });
}
