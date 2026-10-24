# DirectedMST

- `DirectedMST<T>(n)`，点编号 `[0,n)`；`T` 为不超过 64 位的有符号整数。`add(x,y,w)` 添加有向边并返回原边 ID，允许负权、重边和自环。
- `work(root)` 求从固定根出发的最小树形图，返回 `optional<Result>`；无法覆盖全部点时为空。`Result::cost` 是 `__int128_t` 总费用，`edges` 是按终点编号递增排列的 `n-1` 个原边 ID，跳过根。负费用 `-1` 仍是正常答案，不与无解混淆。
- `n >= 0`、边数不超过 `INT_MAX`；`work` 必须有合法根，单点返回零费用和空边集。自环不会被选中；缩环后的权差、堆标记和总费用均用 128 位整数。
- 斜堆维护入边，`RollbackDSU` 收缩环并恢复原边。时间 `O((n+m) log(n+m))` 摊还，空间 `O(n+m)`；堆合并、缩环与恢复都显式遍历，不依赖图或堆的递归深度。
- `work` 使用局部工作状态，可更换根、重复调用或继续 `add`；返回结果拥有数据。

算法与恢复流程改编自 [KACTL DirectedMST.h](https://github.com/kth-competitive-programming/kactl/blob/main/content/graph/DirectedMST.h)，作者 chilli、Takanori MAEHARA、Benq、Simon Lindholm，文件授权 [CC0](https://creativecommons.org/publicdomain/zero/1.0/)。改为索引堆、迭代合并、仓内 RollbackDSU、128 位权值、原边 ID 和 optional 无解接口。

验证：三点边权穷举，枚举每个非根点入边的独立 oracle，随机重边/自环/不可达图，恢复结果与根可达性、64 位极值、5K 层嵌套缩环；20K 层保留为显式 stress case。性能含嵌套缩环和稀疏负权环；仅本机测量，OJ 排名待验证。
