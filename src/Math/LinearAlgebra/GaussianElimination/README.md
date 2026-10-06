# GaussianElimination

[code.hpp](code.hpp)

`gauss(a)` 原地消元；前 n 列为 n×n 系数，其后为一个或多个右端项。所有行同宽且列数大于 n，空矩阵允许。返回 `OK/NoSolution/InfSolution`，OK 时解保存在右端列。

`MatrixUtil<T>(a)` 对方阵求逆，成功时发布 `inv` 和 `status`。T 须为域元素或浮点数，不能用普通整数截断除法；浮点选择最大绝对值主元，零判断仍为精确比较，自定义类型使用自身零比较和求逆语义。

时间 O(n²m)、增广空间 O(nm)，浮点结果受精度及矩阵条件影响。
