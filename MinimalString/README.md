# 最小轮转（GNU++17）

Original 的 `a | minimalString()` 保留，返回最小字典序轮转的拥有型容器。lvalue 复制、rvalue 消耗；string_view 物化为 string；空输入直接返回。

仍使用 i/j/k 的 O(n) 双候选算法。char 按 unsigned byte 比较，避免高位字节的有符号差异；回绕只减一次，省去内层取模。没有独立索引接口或自定义比较器识别框架。依赖：ListHelper。

本轮以 Original 为基准：恢复 Original 最小轮转管道与 i/j/k；删除额外索引接口及自定义比较器框架。保留空输入、拥有 string_view 内容、unsigned 字节顺序及一次回绕优化。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check MinimalString`；性能用 `python3 template/Run.py bench MinimalString`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
