#include "Final.hpp"

#include <array>
#include <cassert>
#include <iostream>

struct PathInfo { long long count = 0; };
constexpr int Bits = 4;
using Tree = BinaryTrie<PathInfo, unsigned, Bits>;

unsigned kth(const Tree& right, const Tree& left, long long k) {
    unsigned res = 0;
    right.walk(left, [&](const auto& a, const auto& b, int dep) {
        if (dep == 0) assert(1 <= k && k <= a.info.count - b.info.count);
        if (dep == Bits) return -1;
        const long long cnt = a[0].info.count - b[0].info.count;
        const int d = k > cnt;
        if (d) k -= cnt;
        res |= unsigned(d) << (Bits - 1 - dep);
        return d;
    });
    return res;
}

// 在 a+b-c-d 表示的集合中查询第 k 小；每个键的组合计数应非负。
unsigned kth(const Tree& a, const Tree& b, const Tree& c, const Tree& d, long long k) {
    unsigned res = 0;
    a.walk(b, c, d, [&](const auto& u, const auto& v, const auto& w, const auto& p, int dep) {
        if (dep == 0) assert(1 <= k && k <= u.info.count + v.info.count - w.info.count - p.info.count);
        if (dep == Bits) return -1;
        const long long cnt = u[0].info.count + v[0].info.count - w[0].info.count - p[0].info.count;
        const int s = k > cnt;
        if (s) k -= cnt;
        res |= unsigned(s) << (Bits - 1 - dep);
        return s;
    });
    return res;
}

int main() {
    static_assert(sizeof(Tree) == 2 * sizeof(int), "Only root and frozen are stored");
    Tree::clearInit();
    auto add = [](PathInfo& info, int) { ++info.count; };
    Tree tree;
    tree.modify(7, add);
    auto old = tree;
    tree.modify(7, add);
    assert(old.query(7).count == 1 && tree.query(7).count == 2);
    const auto uniqueCount = Tree::nodeCount();
    tree.modify(7, [](PathInfo& info, int) { --info.count; });
    assert(Tree::nodeCount() == uniqueCount);

    // 前缀版本：数组 [2,7,9,12]，查询下标区间 [2,4] 的第 2 小。
    const std::array<unsigned, 5> values{0, 2, 7, 9, 12};
    std::array<Tree, 5> prefix;
    for (int i = 1; i <= 4; ++i) {
        prefix[i] = prefix[i - 1];
        prefix[i].modify(values[i], add);
    }
    const unsigned rangeAns = kth(prefix[4], prefix[1], 2);
    assert(rangeAns == 9);

    // 点权：1(5) 的孩子为 2(2)、3(7)，2 的孩子为 4(9)。
    const std::array<int, 5> parent{0, 0, 1, 1, 2};
    const std::array<unsigned, 5> value{0, 5, 2, 7, 9};
    std::array<Tree, 5> version;
    for (int u = 1; u <= 4; ++u) {
        version[u] = version[parent[u]];
        version[u].modify(value[u], add);
    }
    const auto allocated = Tree::nodeCount();
    // 4 到 3，LCA 为 1：root[4]+root[3]-root[1]-root[0]。
    const unsigned pathAns = kth(version[4], version[3], version[1], version[0], 2);
    assert(pathAns == 5 && Tree::nodeCount() == allocated);
    assert(prefix[1].query(7).count == 0 && prefix[4].query(7).count == 1);
    std::cout << "range: second smallest=" << rangeAns << '\n'
              << "path: second smallest=" << pathAns << '\n';
    return 0;
}
