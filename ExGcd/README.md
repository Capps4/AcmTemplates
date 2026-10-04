# ExGcd GNU++17

沿用 Original 的 `exgcd(a,b,x,y)`：返回 gcd，并通过 x/y 引用输出 Bézout 系数。没有 Result 结构体或额外重载。

非负有符号整数；constexpr；T 需容纳中间结果。0,0 返回 {0,1,0}。不是任意精度整数实现。

类型名大驼峰，函数和变量小驼峰；下标 0index，所有区间 [l,r)。函数名保留兼容。

## 验证

std::gcd 和 __int128 Bezout 等式对照，10 万随机输入、零、LLONG_MAX 和编译期验证。

运行 `python3 ../Run.py check ExGcd`（工作目录任意，需将脚本路径指向 template/Run.py）。测试覆盖 GCC -O2 和 Clang ASan/UBSan。

## 性能

20 万对 1e9 内输入；校验和消费 gcd 及全部系数，防止只测 gcd 路径。

输入固定种子、预热一次、每项七轮中位数，旧/新结果校验和必须一致。实际数据见 Benchmark.csv 和 Results.json。仅代表此机器/编译器/工作负载，不推断所有输入下的速度。

## 交付与自我 Review

Final.hpp 为唯一实现来源；验证和交付直接使用头文件。已核对职责保持、API 前置条件、边界、复杂度及新增数据结构必要性。原 snippets 未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check ExGcd`；性能用 `python3 template/Run.py bench ExGcd`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
