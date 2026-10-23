# Dijkstra

[code.hpp](code.hpp)

`Dijkstra<T,G>` 借用带权邻接表，`operator()(s,t)` 查询最短路并按源点缓存。顶点从 0 开始，边权非负；G 默认为 T 与 long long 的公共类型。

不可达或距离达到 G 上限时返回 `Inf`。邻接表须比对象活得久，缓存期间不能修改图。每个源点首次计算 O((n+m) log n)，之后查询 O(1)，每个已计算源占 O(n) 缓存。
