# GaussianElimination

保留 gauss 的字符串状态和 MatrixUtil 的逆矩阵接口。C++17 if constexpr 为普通浮点数选择最大绝对值主元；用 void_t 检测字段元素的 inv()，每行只计算一次逆元再乘法归一化，避免模整数重复求逆。

```cpp
MatrixUtil<Z> matrix(std::vector<std::vector<Z>>{{1,2}, {3,5}});
// status=="OK"；inv={{-5,2}, {3,-1}}（在 Z 的模意义下）
```

gauss 输入 n 行、m>n 列，前 n 列为方阵系数，其余 `[n,m)` 为一个或多个右端项，所有行宽一致。返回 OK 时解位于每行的右端列；奇异系统任何右端项不一致即 NoSolution，否则 InfSolution。矩阵原地修改，非 OK 不发布唯一解。空矩阵为 OK，空方阵逆仍为空。

T 须为域元素或浮点数，不支持普通整数的截断除法。普通 float/double/long double 零判断仍为精确 ==0，主元比较选最大绝对值，不额外引入固定误差阈值；自定义元素按其 ==0 和除法/inv 语义。浮点输入有限，运算结果受浮点精度及可表示范围限制。浮点及没有 inv() 的一般字段继续直接除以缓存主元，避免小浮点主元的显式倒数溢出；复数也保留直接除法路径。具有 inv() 的字段须提供真正的乘法逆元。

MatrixUtil 输入 n×n 方阵，构造增广单位阵，成功才复制右侧为 inv。索引 0index，系数 `[0,n)`、右端 `[n,m)`，维度需可由 int 表示。时间 O(n²m)，增广空间 O(nm)；modular normalization 的逆元次数从按元素降为按行。

测试在 7 元有限域遍历所有 2×2 系数与右端项，枚举全部解作为独立 oracle；检验逆矩阵、奇异状态、多右端项仅后列矛盾。另测随机浮点已知解、左右乘逆矩阵、坏主元顺序、极小/极大尺度、复数及空矩阵。性能见 [Performance.md](Performance.md)：40/100 阶模逆场景明显加速，浮点接近持平；具体倍率按当前报告。

原版 MatrixUtil 的非限定 gauss 调用与最终版、全局 Z 的 ADL 会冲突。Benchmark.cpp 仅用宏将旧 helper 及其调用名改为 legacyGauss，算法与原文件保持，且旧/新都使用同一个最终版 ModuloInteger。原快照未改动。

风格自审：MatrixUtil/HasInverse 大驼峰，gauss 与数学接口保留，局部小驼峰；0index 和半开列范围明确。归一化缓存 scale/factor，不重复求逆，MatrixUtil 明确调用 ::gauss 以防无意 ADL 干扰，没有增加独立矩阵容器或状态层。

本轮以 Original 为基准：Original 主元/消元/回代保留；空输入、多 RHS 无解判定与浮点择主元是修复。四行 void_t 检测支持一行一次 inv 的已测加速；删除巨型矩阵 guard。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check GaussianElimination`；性能用 `python3 template/Run.py bench GaussianElimination`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
