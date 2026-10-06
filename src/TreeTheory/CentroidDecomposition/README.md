# CentroidDecomposition

[code.hpp](code.hpp)

`CentroidDecomposition(adj)` 对无向森林进行重心分解，输出拆分顺序 `dfsOrder`。空森林输出为空；顶点从 0 开始。

组件遍历使用显式栈。时间 O(n log n)，额外空间 O(n)。需要重心树时可启用源码中的 `cdt` 注释选项。
