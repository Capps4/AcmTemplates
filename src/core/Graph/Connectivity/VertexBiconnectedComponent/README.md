# VertexBiconnectedComponent

[code.hpp](code.hpp)

`VertexBC(adj)` 使用 Tarjan 构造圆方树 csqt：原顶点为 `[0,n)`，方节点从 n 开始，孤点只保留圆节点。`componentNum` 为原图连通块数，`csqt[x].size()>1` 可判原顶点割点。

无向边双向存储，支持平行边，自环不单独生成方节点。构造期间借用邻接表，结果属于对象。时间/空间 O(n+m)，递归调用栈最坏 O(n)。
