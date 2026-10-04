# Fenwick（GNU++17）

Original 的 `modify(i,x)`、`posQuery(i)`、`select(k)` 保留；增加的 `rangeQuery(l,r)` 是原注释里的加法半开区间查询。posQuery(-1) 返回 identity；select 返回合并结果 <= k 的最长前缀的包含式末端下标，空前缀为 -1。select 要求前缀合并结果单调。

构造 `Fenwick(n, merge={}, identity={})` 或 `Fenwick(a,merge={},identity={})`，后者 O(n)。merge 需交换、结合且 identity 是单位元；最大值且输入含负数时显式传最低值。modify 是合并增量，不是赋值；Max 更新只允许单调增大。

单次更新/前缀/select 为 O(log n)。删除重复的 size/prefixQuery/maxPrefix 包装。节点规模、下标及结果类型范围由竞赛调用方保证。

本轮以 Original 为基准：恢复 Original posQuery/select 热接口；删除 size/prefixQuery/maxPrefix 包装和 merge 类型验证。保留显式 identity、线性构建和缓存起始步长，空树 select 安全。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check FenwickTree`；性能用 `python3 template/Run.py bench FenwickTree`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
