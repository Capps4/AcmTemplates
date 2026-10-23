# StronglyConnectedComponent

[code.hpp](code.hpp)

`SCC(adj)` 使用递归 Tarjan，公开 `dfn/low/bel/cntBlock/g`。顶点从 0 开始，g 为凝聚图并保留平行边，凝聚边的分量编号从大指向小。

邻接表只在构造期间借用，结果属于对象。时间/空间 O(n+m)，递归调用栈最坏 O(n)，深链需足够的运行栈。
