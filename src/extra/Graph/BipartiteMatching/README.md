# BipartiteMatching

[code.hpp](code.hpp)

`BipartiteMatching(n, m)` 建立左右各 n、m 个点的二分图，编号分别为 `[0,n)`、`[0,m)`；允许空侧、重复边。`add(x,y)` 只加入左到右的边。

`work()` 返回当前最大匹配的总大小；`matchL[x]`、`matchR[y]` 为另一侧编号，未匹配为 -1。两个数组只读使用。可重复 `work`，也可追加边后继续求解；旧匹配保留。

`minCover()` 先完成最大匹配，返回 `{左侧编号列表, 右侧编号列表}`。两表按编号递增，其总大小等于最大匹配数，每条边至少有一个端点入选。新增边后旧覆盖失效，应重新调用。

Hopcroft–Karp 分层增广，显式路径栈避免长交错路径的递归栈限制。单次从空匹配计算为 O((n+m+E)√(n+m))，空间 O(n+m+E)；已有最大匹配上的只读 `work` 和覆盖构造为 O(n+m+E)。n、m、E 及邻接表下标须可用 int 表示，n < INT_MAX。

参考 [KACTL HopcroftKarp](https://github.com/kth-competitive-programming/kactl/blob/main/content/graph/HopcroftKarp.h) 的分层思路；本实现使用显式栈、当前弧和独立的覆盖构造。

正确性用例包含全部至 3×3 的图、独立匹配与覆盖穷举、重边、增量及长交错路径。性能用例用稀疏交错链与四邻接循环图，对照 core MaxFlow 的二分图归约；计时见测试报告，尚无 OJ 排名证据。
