# EdgeBiconnectedComponent

[code.hpp](code.hpp)

`EdgeBC(adj)` 使用 Tarjan 求无向图边双分量及桥森林。无向边双向存储，支持自环和平行边；顶点从 0 开始。

`bel/cntBlock` 为分量归属/数量，`g` 为桥森林，`cutDeg[x]` 为 x 关联的桥数，`componentNum` 为原图连通块数。原边两端 bel 不同即可判桥。

时间和空间 O(n+m)，递归调用栈最坏 O(n)。邻接表仅在构造期间借用；深链需足够的运行栈。
