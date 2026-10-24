# LinkCutTree

`LinkCutTree<Info>(a)` 建立 `n` 个互不相连的点，点权为 `a`。`Info` 可复制、可赋值，`operator+` 满足结合律；不要求默认构造、单位元或交换律。尾部 `// replace me` 的 `Info` 是路径点权和示例，按题目替换。

- `link(x,y)`：连接两个不同连通块，成功返回 `true`；自环或成环返回 `false`。
- `cut(x,y)`：删除已有的直接边；不存在该边时返回 `false`。
- `connected(x,y)`：判断连通性；`modify(x,v)` 替换点权。
- `query(x,y)`：返回 `optional<Info>`，按 **x 到 y** 的方向合并路径点权，含两个端点；不连通时为空。同点查询返回该点权。

点编号 `[0,n)`，构造允许 `n=0`，容量不超过 `INT_MAX` 且受内存限制。仅维护动态森林和路径信息；不维护子树、路径 lazy 或动态 DP，也不改变 core Tree 的根接口。

构造和空间 `O(n)`，操作均摊 `O(log n)`，单次最坏 `O(n)`；时间另乘 `Info` 合并/复制成本。Splay 下传使用显式栈，深链不消耗递归栈。对象拥有状态，复制互不影响；查询也会改变辅助树结构，不返回内部引用。重新构造即重置。

思路参考 [Luzhiled's Link Cut Tree](https://ei1333.github.io/library/structure/dynamic-tree/lazy-link-cut-tree.hpp.html)，参考库许可为 [Unlicense](https://github.com/ei1333/library/blob/master/LICENSE)。本实现独立编写，采用索引节点、正反聚合及显式下传栈，未复制参考库代码，不包含其 lazy 扩展。
