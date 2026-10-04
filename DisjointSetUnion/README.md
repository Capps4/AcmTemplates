# 并查集（GNU++17）

保留 Original 的 `DSU(n)`、`init(n)`、`find(x)`、`Union(x,y)` 和公开 `size`。`Union` 将 y 的代表接到 x 的代表，不按大小交换，保留方向语义。组件大小取 `size[find(x)]`。

路径折半与 Original 一致；不维护额外组件计数，不提供策略模板或附加查询包装。顶点下标合法、n 能放入 int 是使用前提。不把此方向合并版本描述为按大小合并的复杂度。

本轮以 Original 为基准：恢复 Original DSU/Union 合并方向和 f/size/path halving；删除策略模板、DirectionalDSU、merge/same/count 包装及 components 状态。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check DisjointSetUnion`；性能用 `python3 template/Run.py bench DisjointSetUnion`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
