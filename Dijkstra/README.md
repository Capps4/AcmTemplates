# Dijkstra（GNU++17）

保留 Original 的借用图与按源缓存，`Dijkstra<T,G=common_type_t<T,long long>>(g)`、`d(x,y)` 查询距离。简短的 `distances(s)` 返回全部源距离的 const 引用。缓存建立后图应保持不变，图必须比对象活得久。

权重非负，顶点下标合法，距离必须可表示。Inf 是 G 的最大值；扩展前检查加法，避免溢出，实际等于 Inf 的距离无法与不可达区分。使用结构化绑定，没有额外 clearCache 或类型限制框架。单源 O((n+m) log n)，缓存命中查询 O(1)。

本轮以 Original 为基准：Original 借用图及按源缓存，保留宽距离和溢出前判断。删除额外类型限制、每边诊断和 clearCache；保留短全源距离接口。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check Dijkstra`；性能用 `python3 template/Run.py bench Dijkstra`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
