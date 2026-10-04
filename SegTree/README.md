# SegTree GNU++17

`SegTree<Info, Tag = void>`：同层叶子、自底向上操作的迭代线段树。`Tag=void` 时不存标记，不要求 `Info::apply`，也不提供区间 `modify(l,r,tag)` 重载。

所有下标从 0 开始，区间均为 `[l,r)`。

```cpp
SegTree<Info> plain(a);
SegTree<Info, Tag> lazy(a);
SegTree<Info, Tag> tree(n, leaf);

tree.modify(p, value);       // 单点赋值
tree.query(l, r);
lazy.modify(l, r, tag);
tree.first(l, r, pred);
tree.last(l, r, pred);
```

`Info` 可从输入元素构造，支持复制和赋值，不要求默认构造。`operator+` 只需满足结合律，不需要单位元；合并保持左右顺序，不要求交换律。`leaf` 是真实叶子的初值，例如区间和的叶子长度为 1。

建树和回拉只合并真实叶子；右孩子全是补齐位置时直接取左孩子。底层未使用的槽位以首个叶子的副本初始化，不参与合并和查询。区间查询首次命中节点时直接取该节点的信息，只在已有结果时合并；左右均有结果才进行最后一次合并。

Lazy 模式要求 `Tag()` 为空操作、`Info::apply(tag)` 更新区间信息、`Tag::apply(after)` 合成“先旧操作、后新操作”。标记作用必须与合并兼容。仅内部节点存标记；不要求标记支持相等比较。

头文件尾部附带全局 `Info` / `Tag` 示例，并以 `replaceme` 注释提示按题目改写。辅助命名空间为 `_segt`。直接使用默认的区间加、区间和示例：

```cpp
SegTree<Info, Tag> tree(std::vector<int>{1, 2, 3});
tree.modify(0, 2, Tag{4});
assert(tree.query(0, 3).val == 14);
```

空树可构造，但 `query` 要求 `0 <= l < r <= n`；违反查询前置条件时通过 `assert` 拒绝。空修改无操作，空搜索返回 `std::nullopt`。构造要求 `0 <= n <= INT_MAX/2`，填充构造直接委托 `std::vector<Info>(n, leaf)`；单点操作要求 `0 <= p < n`。

搜索返回 `std::optional<int>`：有答案时包含范围内首个／末个满足 `pred(leaf)` 的下标，无解返回 `std::nullopt`。下标 0 是有效答案；用 `if (auto p = tree.first(l,r,pred))` 判断，再用 `*p` 取得下标；需要整数哨兵时可用 `.value_or(-1)`。谓词对节点判假时，必须保证其所有叶子都不满足条件；允许节点假阳性，内部会回溯。谓词接收 `const Info&`，内部按引用传递，支持以右值传入不可复制谓词。空范围不调用谓词。

构造和空间为 `O(base)`，其中 `base` 是不小于 `max(n,1)` 的最小二次幂；单点操作、区间更新查询为 `O(log n)`。搜索在谓词精确表达“区间内存在匹配叶子”时为 `O(log n)`，例如 `max >= x`；只有剪枝约束时最坏 `O(n)`。复杂度假定合并、标记、复制和谓词为常数时间。搜索不是前缀累计查询。

## 验证

```bash
python3 template/Run.py check SegTree
python3 template/Run.py bench SegTree
```

测试覆盖空树、非二次幂、所有小区间、普通／Lazy 随机数组对拍、非交换 affine 标记、字符串有序合并与区间赋值、带假阳性的搜索、不可复制谓词、optional 下标 0／空范围／无解、无需默认构造且不允许空值的 Info、`vector<bool>` 和 `Tag=void` 编译期接口限制。运行报告在 `../Reports/`。

当前版本通过严格编译、优化版对拍和 ASan/UBSan，见[模块报告](../Reports/20261004T022848Z-bd8bc8a1/Summary.md)；头文件共存与程序入口检查见[集成报告](../Reports/20261004T022954Z-827667b7/Summary.md)。模块间的 ExamplesTest.cpp 也通过严格编译、优化版和 ASan/UBSan。

`Original.hpp` 内保留旧 LazySegmentTree 实现快照，仅作同期性能对照；旧模块目录已移除。四个场景为建树、区间更新查询、单点赋值查询和最大值左右搜索；普通模式对照旧 Lazy 树，场景名已明确标出。输入预生成、每次回调重建树，校验 checksum 后交替计时、七轮取中位数；不将不同场景倍率合成一个数字。

此前含单位元版本的性能报告属于历史记录；本版[同期性能报告](../Reports/20261004T023008Z-7cac8de5/Summary.md)已重新编译计时，结果仅限报告中的机器、编译器和工作负载。

## 参考

- [AtCoder lazysegtree 源码](https://github.com/atcoder/ac-library/blob/master/atcoder/lazysegtree.hpp)：边界路径下推，以及只回拉部分覆盖的祖先。
- [AtCoder 契约](https://atcoder.github.io/ac-library/production/document_en/lazysegtree.html)：结合律和标记作用；本实现只支持非空查询，因此不要求 Info 单位元。
- [OI Wiki 线段树基础](https://oi-wiki.org/ds/seg/)：非递归布局和同层叶子；本实现按实际 n 跳过补齐位置。

按现有成员式 `Info/Tag` 接口独立实现；`first/last` 采用叶子匹配语义。

测试使用 SumInfo / AddTag 等独立类型；Benchmark 使用 BenchInfo / BenchTag，旧递归实现放在 legacy 命名空间，由 RefTree 仅适配接口名供性能对照。ExamplesTest.cpp 与集成检查仅在验证源码中重命名尾部示例，使三份模板可以共同编译；交付头文件保留 Info / Tag 名称。

模块间示例测试独立运行：`g++ -std=c++17 -O2 template/SegTree/ExamplesTest.cpp -o /tmp/tree-examples && /tmp/tree-examples`。
