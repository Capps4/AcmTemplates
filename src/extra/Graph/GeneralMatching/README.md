# GeneralMatching

- `GeneralMatching(n)`，点编号 `[0,n)`；`add(x,y)` 添加无向边。自环忽略，重边允许。求一般图的无权最大基数匹配。
- `work()` 返回匹配边数，`mate[x]` 为配对点，未匹配为 `-1`。只添加边后可再次 `work`，沿用已有合法匹配；外部不要修改 `mate`。无随机预匹配，同一插入顺序的结果确定。
- Edmonds 缩花，队列搜索、花内路径标记和增广全部显式遍历。每次搜索 `O(m+n²)`，总计 `O(nm+n³)`；简单图为 `O(n³)`。空间 `O(n+m)`。大量重边会增加扫描成本，调用方可先去重。
- 仅最大基数匹配，不包含带权匹配；二分图优先使用 `BipartiteMatching`。

算法参考：[Luzhiled Gabow–Edmonds](https://ei1333.github.io/library/graph/flow/gabow-edmonds.hpp)（[Unlicense](https://github.com/ei1333/library/blob/master/LICENSE)）。本实现采用经典显式 base/LCA 缩花，不搬入其递归 rematch 内核；不使用随机 Tutte 矩阵法。

验证：常规穷举至多五点全部简单图，以顶点子集枚举匹配作独立 oracle；六点全图保留为显式 stress case。另有随机重边/自环、花与增量加边、2K 交错长路。性能含稀疏奇环链和稠密图；仅记录本机结果，OJ 排名待验证。
