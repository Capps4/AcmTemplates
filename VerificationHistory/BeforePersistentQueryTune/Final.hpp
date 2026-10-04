#pragma once
#include <cassert>
#include <functional>
#include <limits>
#include <utility>
#include <vector>
#include "../Discreter/Final.hpp"

// SNIPPET BEGIN
template<class T, class Compare = std::less<>>
class PersistentTree {
    struct Node { int count = 0, left = 0, right = 0; };
    const int n;
    const Discreter<T, Compare> disc;
    std::vector<Node> nodes;
    std::vector<int> roots;
    static int checkedSize(const std::vector<T>& input) {
        assert(input.size() < std::size_t(std::numeric_limits<int>::max()));
        return int(input.size());
    }
    int pushBack(int original, int l, int r, int rank) {
        int node = int(nodes.size());
        nodes.push_back(nodes[original]);
        ++nodes[node].count;
        if (r - l > 1) {
            int mid = l + (r - l) / 2;
            if (rank < mid) nodes[node].left = pushBack(nodes[node].left, l, mid, rank);
            else nodes[node].right = pushBack(nodes[node].right, mid, r, rank);
        }
        return node;
    }
    int query(int x, int y, int l, int r, int tl, int tr) const {
        if (tl <= l && r <= tr) return nodes[y].count - nodes[x].count;
        int mid = l + (r - l) / 2, result = 0;
        if (tl < mid) result += query(nodes[x].left, nodes[y].left, l, mid, tl, tr);
        if (mid < tr) result += query(nodes[x].right, nodes[y].right, mid, r, tl, tr);
        return result;
    }
    int rankRange(int l, int r, int tl, int tr) const {
        assert(0 <= l && l <= r && r <= n);
        if (l == r || tl == tr) return 0;
        return query(roots[l], roots[r], 0, disc.size(), tl, tr);
    }
public:
    explicit PersistentTree(const std::vector<T>& input, Compare cmp = {})
        : n(checkedSize(input)), disc(input, std::move(cmp)), nodes(1), roots(std::size_t(n) + 1) {
        if (n == 0) return;
        int levels = 0;
        for (unsigned width = 1; width < unsigned(disc.size()); width *= 2) ++levels;
        assert(std::size_t(n) <= std::size_t(std::numeric_limits<int>::max() - 1) / (levels + 1));
        nodes.reserve(1 + std::size_t(n) * (levels + 1));
        for (int i = 0; i < n; ++i) roots[i + 1] = pushBack(roots[i], 0, disc.size(), disc.rankOf(input[i]));
    }
    int size() const { return n; }
    // Position [l,r), value [tl,tr) in comparator order.
    int range(int l, int r, const T& tl, const T& tr) const {
        assert(!disc.comparator()(tr, tl));
        return rankRange(l, r, disc.rankOf(tl), disc.rankOf(tr));
    }
    int countLess(int l, int r, const T& value) const {
        return rankRange(l, r, 0, disc.rankOf(value));
    }
    int countLessEqual(int l, int r, const T& value) const {
        return rankRange(l, r, 0, disc.upperRankOf(value));
    }
    typename Discreter<T, Compare>::ConstReference upTo(int l, int r, int k) const & {
        assert(0 <= l && l < r && r <= n && 0 <= k && k < r - l);
        int x = roots[l], y = roots[r], lo = 0, hi = disc.size();
        while (hi - lo > 1) {
            int mid = lo + (hi - lo) / 2;
            int leftCount = nodes[nodes[y].left].count - nodes[nodes[x].left].count;
            if (k < leftCount) { x = nodes[x].left; y = nodes[y].left; hi = mid; }
            else { k -= leftCount; x = nodes[x].right; y = nodes[y].right; lo = mid; }
        }
        return disc.at(lo);
    }
    T upTo(int l, int r, int k) const && {
        return static_cast<const PersistentTree&>(*this).upTo(l, r, k);
    }
};
