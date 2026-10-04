# MoSort GNU++17

Query(l,r,id)，按分块蛇形顺序排序；rangeScale(n) 设置共享块长。

区间 [l,r)，内联静态块长消除类外定义，比较按 const 引用；2.0*n 避免 n*2 的 int 溢出。块长保持原来的启发式，不声称是最优取值。

类型名大驼峰，函数和变量小驼峰；下标 0index，所有区间 [l,r)。函数名保留兼容。

## 验证

独立排序键对照，0/1/大规模及 INT_MAX，比较器自反性和严格弱序。

运行 `python3 ../Run.py check MoSort`（工作目录任意，需将脚本路径指向 template/Run.py）。测试覆盖 GCC -O2 和 Clang ASan/UBSan。

## 性能

20 万随机查询排序，排序前复制、排序和输出校验均对称。共享块长修改后必须重排，不支持同时使用多种块长。

输入固定种子、预热一次、每项七轮中位数，旧/新结果校验和必须一致。实际数据见 Benchmark.csv 和 Results.json。仅代表此机器/编译器/工作负载，不推断所有输入下的速度。

## 交付与自我 Review

Final.hpp 为唯一实现来源；验证和交付直接使用头文件。已核对职责保持、API 前置条件、边界、复杂度及新增数据结构必要性。原 snippets 未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check MoSort`；性能用 `python3 template/Run.py bench MoSort`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
