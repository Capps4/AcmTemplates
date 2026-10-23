# FenwickTree

[code.hpp](code.hpp)

`Fenwick` 使用 0 基下标。`modify(i,v)` 合并点值；`query(i)` 查询包含 i 的前缀，`query(-1)` 返回单位元。区间结果由调用方使用前缀差计算。

Merge 须满足交换、结合律，unit 为双侧单位元。`select(lim)` 返回合并值不大于 lim 的最长前缀末端，空前缀为 -1，要求前缀单调。

线性构建 O(n)，修改、查询和选择 O(log n)，空间 O(n)。
