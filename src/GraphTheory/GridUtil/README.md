# GridUtil

[code.hpp](code.hpp)

`GridUtil(n,m)` 提供 `contains/neighbors/forEachNeighbor(x,y,f)`。网格坐标从 0 开始，默认方向顺序为左、右、上、下。

D、方向数组、循环上界及 reserve 共用 `D=4`；改八方向时将 D 改为 8 并补齐 dx/dy。neighbors 分配结果 vector，forEachNeighbor 不分配。每次邻居遍历 O(D)。
