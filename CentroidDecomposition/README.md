# 重心分解（GNU++17）

输入无向森林，输出 Original 的 `dfsOrder`，为空时输出为空。保持组件求大小、找重心、依次拆分的算法。

组件遍历使用数组栈，避免 200K 长链递归爆栈；每次组件的临时 parent/size 数组不作为对象输出。没有新增 centroidParent 常驻数组。O(n log n) 时间、O(n) 额外空间。

本轮以 Original 为基准：保留重心拆分算法；仅组件遍历改显式栈以处理长链。恢复仅 dfsOrder 输出，删除 centroidParent 常驻数组与对应 pending pair。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check CentroidDecomposition`；性能用 `python3 template/Run.py bench CentroidDecomposition`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
