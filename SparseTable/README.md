# RMQ（GNU++17）

`RMQ<T,Cmp>(a,cmp={})` 与 `rmq(l,r)` 返回区间 [l,r) 的最优值。默认 less 为最小值，greater 为最大值。非空查询需合法下标。空数组可构造。

保持 Original 的层数组、resize 构造和按值结果，T 需要可默认构造/复制。比较器通过 cref 使用，避免有状态比较器在每次 min 调用中复制，也允许 move-only 比较器。建表 O(n log n)，查询 O(1)。没有引用结果的额外生命周期规则。

本轮以 Original 为基准：恢复 Original RMQ 字段/层数组/resize/按值查询；删除非默认构造分支及引用返回框架。保留空输入、状态比较器和 cref 避免昂贵复制。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check SparseTable`；性能用 `python3 template/Run.py bench SparseTable`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
