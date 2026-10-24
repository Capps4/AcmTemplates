# CentroidDecomposition

[code.hpp](code.hpp)

`CentroidDecomposition(adj)` 对无向森林进行重心分解，输出拆分顺序 `dfsOrder`。空森林输出为空；顶点从 0 开始，邻接表双向、无自环或重边。

可用 `CentroidDecomposition(adj, &cdt)` 同时输出点分树：cdt 被覆盖为 n 个邻接表，每条边从点分父亲指向点分儿子，各分量根没有父亲。cdt 不得与输入 adj 为同一对象。默认调用不保存点分树；算法结束后不借用输入或输出引用。

组件遍历使用显式栈，待处理任务携带点分父亲，与组件遍历的 fa 分离。时间 O(n log n)，额外空间 O(n)；邻接表长度及顶点数须可用 int 表示。

测试用独立连通分量搜索检验每步重心平衡，并核对可选点分树的父子关系、森林根和默认模式顺序。
