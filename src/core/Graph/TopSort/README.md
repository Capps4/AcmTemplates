# TopSort

[code.hpp](code.hpp)

`topSort(adj)` 使用 Kahn 算法，结果 vector 同时作为 FIFO 队列。支持 int 邻居与 `pair<int,Weight>` 边，不复制权重。

顶点为 `[0,n)`；结果大小等于 n 当且仅当无环，有环时只返回可移除前缀。空图返回空序列，顺序不保证字典序最小。时间 O(n+m)、额外空间 O(n)。
