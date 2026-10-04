#include "Final.hpp"

#include <array>
#include <cassert>
#include <iostream>

struct DemoInfo {
    long long count = 0, sum = 0;
    DemoInfo operator+(const DemoInfo& b) const { return {count + b.count, sum + b.sum}; }
    DemoInfo operator-(const DemoInfo& b) const { return {count - b.count, sum - b.sum}; }
};

int main() {
    using Tree = SparseSegTree<DemoInfo, long long>;
    static_assert(sizeof(Tree) == 2 * sizeof(int), "Root and frozen boundary are stored per tree");
    Tree::clearInit(0, 16);
    {
        const auto add = [](Tree tree, long long value) {
            tree.modify(value, [value](DemoInfo old) {
                ++old.count;
                old.sum += value;
                return old;
            });
            return tree;
        };
        const auto atLeast = [](long long k) {
            return [k](const DemoInfo& info) { return info.count >= k; };
        };

        // 默认修改当前对象的根，通过 COW 保护已经保存的历史版本。
        Tree empty;
        const Tree first = add(empty, 2);
        const Tree second = add(first, 7);
        const Tree branch = add(first, 9);
        Tree assigned = first;
        assigned.modify(2, DemoInfo{3, 6}); // 自动保护与 first 共享的节点。
        assert(first.query(0, 16).count == 1 && assigned.query(0, 16).count == 3);

        // 差分视图使用与单树相同的 query / first / last。
        const auto diff = second - first; // 只有一个 7。
        assert(diff.query(0, 16).sum == 7);
        assert(diff.first(0, 16, atLeast(1)) == 7);
        assert(diff.last(0, 16, atLeast(1)) == 7);
        assert(!diff.first(0, 16, atLeast(2)));
        assert(!diff.last(5, 5, atLeast(1)));
        assert(!empty.first(0, 16, atLeast(1)));
        assert(diff.query(5, 6).count == 0);

        // 默认合并叶子相加；想保留两个输入，就先复制出结果对象。
        Tree merged = second;
        merged.merge(branch); // {2,2,7,9}
        assert(merged.query(0, 16).count == 4 && merged.query(0, 16).sum == 20);
        assert(merged.first(0, 16, atLeast(3)) == 7); // 第 3 小。
        assert(merged.last(0, 16, atLeast(2)) == 7); // 第 2 大。
        assert(merged.first(3, 10, atLeast(2)) == 9); // 限制搜索范围。
        assert(merged.last(3, 10, atLeast(2)) == 7);
        assert(second.query(0, 16).count == 2 && branch.query(0, 16).count == 2);
        Tree doubled = second;
        doubled.merge(doubled);
        assert(doubled.query(0, 16).count == 4);
        Tree unique = second;
        unique.merge(branch, [](const DemoInfo& a, const DemoInfo& b, long long) {
            return a.count >= b.count ? a : b;
        });
        assert(unique.query(0, 16).count == 3);

        // 四根树上查询：1(5) 的孩子是 2(2)、3(7)，2 的孩子是 4(9)。
        const std::array<int, 5> parent{0, 0, 1, 1, 2};
        const std::array<long long, 5> value{0, 5, 2, 7, 9};
        std::array<Tree, 5> version;
        for (int u = 1; u <= 4; ++u) version[u] = add(version[parent[u]], value[u]);
        const auto path = version[4] + version[3] - version[1] - version[0];
        assert(path.query(0, 16).count == 4 && path.query(0, 16).sum == 23);
        assert(path.first(0, 16, atLeast(2)) == 5);
        assert(path.last(0, 16, atLeast(2)) == 7);
        // 保存根快照的视图不会随 COW 修改改变。
        version[4].modify(1, DemoInfo{1, 1});
        assert(path.query(0, 16).count == 4);
        std::cout << "path: sum=" << path.query(0, 16).sum
                  << ", second smallest=" << *path.first(0, 16, atLeast(2)) << '\n';

        // 独占节点自动原地复用；普通查询不会使节点变成共享。
        Tree a, b;
        a.modify(3, DemoInfo{1, 3});
        b.modify(3, DemoInfo{1, 3});
        b.modify(8, DemoInfo{1, 8});
        assert(b.query(0, 16).sum == 11);
        b.modify(8, [](DemoInfo old) { return old; });
        const Tree saved = b;
        a.merge(std::move(b));
        assert(a.query(0, 16).sum == 14 && b.query(0, 16).count == 0);
        a.modify(8, DemoInfo{2, 16});
        assert(saved.query(0, 16).sum == 11); // 被消费右树的历史版本仍然有效。

        // 冻结后只复制第一次触及的旧路径；同一路径继续修改时复用新节点。
        Tree c;
        c.modify(3, DemoInfo{1, 3});
        c.modify(12, DemoInfo{1, 12});
        const auto snap = c.view();
        c.modify(3, DemoInfo{2, 6});
        c.modify(3, DemoInfo{3, 9});
        c.modify(12, DemoInfo{2, 24});
        assert(snap.query(0, 16).sum == 15 && c.query(0, 16).sum == 33);
        Tree d = std::move(c);
        d.modify(12, DemoInfo{3, 36});
        assert(c.query(0, 16).count == 0);
        assert(snap.query(0, 16).sum == 15 && d.query(0, 16).sum == 45);
    }
    // 重置值域并清池。optional 允许 -1 本身是一个有效答案。
    Tree::clearInit(-8, 8);
    Tree fresh;
    fresh.modify(-1, DemoInfo{1, -1});
    assert(fresh.first(-8, 8, [](const DemoInfo& x) { return x.count >= 1; }) == -1);
}
