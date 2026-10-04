# 异或线性基（GNU++17）

Original 的 `b/rank/canZero`、insert/check/getMax/getMin/clear/findByOrder 保留。`findByOrder(k)` 为非空子集 distinct XOR 结果的 0-based 排名，调用方保证 k 有效；零是否存在取决于 canZero。

支持非负 signed 和完整 unsigned 位域，直接按 T 检查位；signed 输入限定非负，不需要额外 Unsigned 类型或转换。dirty 在 findByOrder 前触发约化，后续 insert 会重新标记。没有 optional kth/includeEmpty 的额外接口。插入/查询 O(bits)，约化 O(bits²)。

本轮以 Original 为基准：保留 Original 基底/dirty/rank/canZero 和 findByOrder(T)，删除多余 Unsigned alias。若允许空子集，可以注释掉 k += !canZero，保留 Original 的功能切换方式。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check LinearBasis`；性能用 `python3 template/Run.py bench LinearBasis`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
