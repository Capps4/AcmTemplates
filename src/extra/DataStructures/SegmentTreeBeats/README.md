# SegmentTreeBeats

数值数组的区间 `chmin(l,r,x)`、`chmax(l,r,x)`、`add(l,r,x)`，以及 `query(l,r)` 返回的 `sum/mn/mx`。下标从 0 开始，区间为 `[l,r)`；允许空数组和空修改，查询必须非空。

真实元素值与实际区间和须可表示为 `long long`，允许元素取 `LLONG_MIN/LLONG_MAX`。累计加法标记、截断差值乘法及下传临时量使用 128 位；下传先在宽类型中完成平移与截断，再写回，避免陈旧子节点的瞬时溢出。累计标记及其乘长度须可表示为 128 位。长度不超过 `INT_MAX/4`，还受可用内存限制。

构造 `O(n)`、空间 `O(n)`。查询 `O(log n)`；混合截断与加法的修改序列为均摊 `O(log² n)`，单次修改可能访问 `O(n)` 个节点。递归深度 `O(log n)`。对象拥有数组状态，复制互不影响；重新构造即重置。

思路参考 [Nyaan's Segment Tree Beats](https://nyaannyaan.github.io/library/segment-tree/segment-tree-beats.hpp.html)，参考库许可为 [CC0-1.0](https://github.com/NyaanNyaan/library/blob/master/LICENSE)。本实现独立编写，采用递归边界和宽类型下传，未复制参考库代码。
