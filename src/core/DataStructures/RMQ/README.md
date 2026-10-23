# RMQ

[code.hpp](code.hpp)

`RMQ<T,Cmp>(a,cmp={})` 使用稀疏表，`rmq(l,r)` 返回非空区间 `[l,r)` 的最优值；默认 less 为最小值，greater 为最大值。

空数组允许构造；查询下标须合法，T 须可默认构造与复制。比较器固定且满足严格弱序，结果按值返回。建表时间/空间 O(n log n)，查询 O(1)。
