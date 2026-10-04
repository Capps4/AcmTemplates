# 离散化（GNU++17）

`Discreter<T> d(std::vector<T>)` 排序去重后拥有 keys。只提供 `size()`、`rankOf(x)`、`at(i)`、`begin()/end()`；`rankOf` 是 lower_bound 插入位置，未出现的键也可查询。`at` 返回值。

Original 的 `a | discreteFrom(keys, cmp)` 保留：keys 需按同一比较器有序；lvalue keys 被借用，rvalue keys 被拥有，缓存算子可反复使用。Discreter 使用 ListHelper 的 `sorted()`，没有再实现一套排序。

依赖：ListHelper。PersistentTree 的 Original 使用了缺失的 Discreter 类型，因此仍补这个最小容器。

本轮以 Original 为基准：恢复 Original discreteFrom 管道；为 Original PersistentTree 缺失类型补最小 Discreter(size/rankOf/at/begin/end)。删除比较器容器框架、upperRankOf/contains/traits dispatch/引用返回分支。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check Discreter`；性能用 `python3 template/Run.py bench Discreter`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
