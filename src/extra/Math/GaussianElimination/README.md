# GaussianElimination

[code.hpp](code.hpp)

`gauss(a)` 保留方阵接口：前 n 列为 n×n 系数，后面是至少一个右端项；所有行同宽。返回 `OK/NoSolution/InfSolution`，任何 RHS 无解即返回 `NoSolution`；`OK` 时原地得到单位系数矩阵，解保存在右端列。空矩阵返回 `OK`。

`gauss(a, m)` 处理 n 行、m 个未知量和任意数量 RHS，原地化为行阶梯形，返回 `GaussResult<T>`：

- `rank` 和递增的 `piv`：系数矩阵的秩及主元列。
- `status[k]`：第 k 个 RHS 的 `OK/InfSolution/NoSolution`。
- `sol[k]`：第 k 个 RHS 的一个长度 m 的特解，自由变量为零；无解时为空。

列数须至少为 m，允许 m=0 或没有 RHS。空矩阵没有可推断的 RHS，返回 rank=0、空 piv/status/sol。行列数使用 int 表示。所有索引从 0 开始。

`MatrixUtil<T>(a)` 对方阵求逆，成功时发布 `inv` 和 `status`。它与旧接口复用消元内核，不构造额外的特解结果。

T 是域元素或浮点数，默认构造为零、`T(1)` 为单位元；普通整数除法不合法。支持 `std::complex` 和具有 `inv()` 的域类型；有 `inv()` 时每个主元只求逆一次。浮点选择最大绝对值主元，保留精确零判断，不引入固定 epsilon；自定义类型沿用自身零比较。浮点/复数结果受精度、溢出和矩阵条件影响，输入与中间值需可表示。

设 r=min(n,m)、w=总列数、q=RHS 数。消元 O(nrw)，特解提取 O(rmq)，结果空间 O(r+mq)，原地消元额外空间 O(r)。旧方阵接口时间 O(n²w)，MatrixUtil 增广空间 O(n²)。

参考：[KACTL SolveLinear](https://github.com/kth-competitive-programming/kactl/blob/main/content/numerical/SolveLinear.h) 的矩形系统语义；保留本仓的多 RHS、复数和极小/极大浮点尺度路径。新增测试覆盖矩形、多 RHS、无解与自由变量。
