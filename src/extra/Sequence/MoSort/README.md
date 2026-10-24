# MoSort

[code.hpp](code.hpp)

`Query(l,r,id)` 使用 0 基半开区间 `[l,r)`，须满足 `0 <= l <= r <= n`，id 为原查询编号。`n` 和查询数均为非负 int。

```cpp
std::sort(q.begin(), q.end(), MoSort(n, int(q.size())));
```

比较器按分块蛇形顺序排列查询，块内右端交替升降；每个比较器独立保存块长 `max(1, floor(n/sqrt(max(1,Q))))`，可交错处理不同数据集。替代原来的共享 `Query::rangeScale` 和 `operator<`；Query 的数据字段保持不变。

排序 O(Q log Q)，比较器空间 O(1)。端点维护由调用方处理；块长是等成本端点操作下的选择，不承诺任意分布最优。

参考：[KACTL MoQueries](https://github.com/kth-competitive-programming/kactl/blob/main/content/data-structures/MoQueries.h)、[Luzhiled Mo](https://github.com/ei1333/library/blob/master/other/mo.hpp)。
