# MoSort

[code.hpp](code.hpp)

`Query(l,r,id)` 使用 0 基半开区间 `[l,r)`，id 为原查询编号；`Query::rangeScale(n)` 设置共享块长。

比较器按分块蛇形顺序排列查询，排序后块内右端交替升降。块长为启发式取值，处理新数据前重新设置；Q 个查询排序 O(Q log Q)，移动端点成本由调用方操作决定。
