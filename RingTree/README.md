# 环树（GNU++17）

Original 两种构造保留：无向每组件最多一个环的图，或每点唯一出边的 link 数组。输出 `g`（剥叶形成的树边）与 `rings`。支持纯树/孤点、合法自环和平行双边环。

修复 Original 在纯树最后一条边上的丢失；环顶点不列入剥叶树的子节点。没有新增 parent 常驻数组；需要父关系时自行从 g 推导。整体 O(n+m)。

本轮以 Original 为基准：Original 剥叶/找环与 g/rings 输出保留；修复纯树末边丢失。删除新增 parent 常驻数组与容量断言。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check RingTree`；性能用 `python3 template/Run.py bench RingTree`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
