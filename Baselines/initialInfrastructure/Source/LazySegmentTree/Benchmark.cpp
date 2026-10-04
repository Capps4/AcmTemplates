#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
struct Operation { int l, r, delta; };
int main() {
    constexpr int n = 100000;
    std::vector<int> values(n, 1);
    std::mt19937 rng(20261001);
    std::vector<Operation> operations(100000);
    for (auto& [l, r, delta] : operations) {
        l = rng() % n; r = l + 1 + rng() % (n - l); delta = rng() % 10;
    }
    compare("build-100K-x20", [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20; ++i) { Legacy::LazySegT<Legacy::Node, Legacy::Tag> tree(values); sum += tree.rangeQuery(0, n).val; }
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20; ++i) { LazySegT<Node, Tag> tree(values); sum += tree.rangeQuery(0, n).val; }
        return sum;
    });
    auto work = [&](auto& tree, auto makeTag) {
        std::uint64_t sum = 0;
        for (auto [l, r, delta] : operations) { tree.rangeModify(l, r, makeTag(delta)); sum += tree.rangeQuery(l, r).val; }
        return sum;
    };
    compare("range-update-query-100K", [&] {
        Legacy::LazySegT<Legacy::Node, Legacy::Tag> tree(values);
        return work(tree, [](int delta) { return Legacy::Tag(delta); });
    }, [&] {
        LazySegT<Node, Tag> tree(values);
        return work(tree, [](int delta) { return Tag(delta); });
    });
    auto pred = [thresholds = std::vector<int>(256, 0)](const auto& info) { return info.val > thresholds[0]; };
    Legacy::LazySegT<Legacy::Node, Legacy::Tag> oldTree(values);
    LazySegT<Node, Tag> tree(values);
    compare("search-vector-capture-20K", [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20000; ++i) sum += oldTree.findFirst(i, n, pred);
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20000; ++i) sum += tree.findFirst(i, n, pred);
        return sum;
    });
}
