# ImplicitTreap

`ImplicitTreap<Info>(a, seed)` 维护可变长序列。`Info` 可复制、可赋值，`operator+` 满足结合律；不要求默认构造、单位元或交换律。尾部 `// replace me` 的 `Info` 是区间和示例，按题目替换。节点保存正反聚合，翻转后查询仍按当前位置从左到右合并。

- `insert(p,v)`：在下标 `p` 前插入，允许 `p == size()`。
- `erase(l,r)`、`reverse(l,r)`：删除或翻转 `[l,r)`，允许空区间。
- `query(l,r)`：返回非空 `[l,r)` 的聚合；`size()/empty()` 查询长度。

下标从 0 开始。对象拥有元素，复制互不影响，查询会调整内部结构；不返回内部引用。删除槽位会复用，但其中 `Info` 占用的资源保留到槽位复用或对象析构；容量与历史同时存活元素数的峰值同阶，少于 `INT_MAX` 个节点。重新构造即重置。

Cartesian 单调栈构造为 `O(n)`。插入、翻转、查询对随机优先级的期望时间为 `O(log n)`，删除 `k` 个元素为期望 `O(log n+k)`；单次最坏 `O(n)`。上述时间另乘 `Info` 的合并/复制成本。使用迭代分裂与合并，不依赖递归栈。默认种子取 `steady_clock`；显式传种子可复现，期望界按随机种子理解，不保证对已知种子的对抗输入。空间为历史峰值 `O(n)`。

思路参考 [cp-algorithms 的 Implicit Treap](https://cp-algorithms.com/data_structures/treap.html#implicit-treaps)，文章许可为 [CC BY-SA 4.0](https://github.com/cp-algorithms/cp-algorithms/blob/master/LICENSE)。本实现独立编写，使用索引池、迭代分裂合并及正反聚合，未复制文章代码或文字。
