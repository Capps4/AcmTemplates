# 共享排序：sort / sorted

这是独立排序首次接入、TreeMap 为 592 行时的历史实验。当前已将排序精简并内置到 `_treemap`；此处保留原独立排序实验，旧实现和检查见 [legacy](legacy/)。TreeMap 的 optional 接口与后续精简见 [新报告](../apiSimplification/README.md)。

2026-09-30，macOS arm64、GCC 15.2，`-std=gnu++17 -O2 -DNDEBUG`。正式实现为 [历史 sort.hpp](legacy/sort.hpp)，TreeMapOff 通过 `sorting::sort(keys, compare)` 复用它；SBT 算法未改。

## 接口与分派

`sort(a, cmp)` 原地排序；`sorted(a, cmp)` 返回同类型容器，左值输入复制、右值输入移动。两者均默认升序。

随机访问容器先检查已有序，再检查反向有序；反向有序直接 reverse。只有非 bool 整数、标准 less/greater（含透明比较器）会考虑基数排序。使用 unsigned 的“减去最小值”规范化负数和端点，轮数按实际值域计算；比较 `n × floor(log2(n))` 与 `n + P × (n + 2^B)`，选择 8 位或 11 位一轮。最低估计成本已高于比较排序时，跳过 min/max 扫描。其他情况使用 std::sort；list/forward_list 使用成员 sort。

原地 vector 排序保持 data 指针和容量；基数排序仍会分配一个等长临时数组。临时桶数组最多 2048 个 size_t，arm64 上约 16 KB。原先通过交换 vector 散射的方法不能保证原存储保留，因而没有照搬。排序不承诺等价元素的稳定性。

成本估计是简单分派规则，不是按机器校准的耗时模型，不保证每个输入都最快。共享版额外做顺序检查和范围扫描，因此可能慢于原先专门针对 TreeMap 的固定 8 位实现。

## 独立实测

每组同进程比较 std::sort、原 TreeMapOff 专用 8 位排序、当前共享排序。类型 int/long long；规模 16、64、256、512、1024、2048、4096、16384、65536、100 万、200 万；分布为 22 位非负、16 位正负、全位宽、16 种值和逆序。

每个方案预热一次，随机顺序测 5 轮；小规模每轮重复 `max(1, min(64, 8192/n))` 次，记录平均耗时。输入复制与输出校验均在计时外；所有输出与独立 std::sort 结果逐元素对比。110 个数据组共 1650 条正式样本全部通过。桌面环境未隔离，需参考原始单轮波动。

以下为 **200 万元素的中位数 ns/元素，越小越快**：

| 类型 / 分布 | std::sort | 原专用 8 位 | 共享自适应 |
| --- | ---: | ---: | ---: |
| int / 22 位非负 | 41.548 | 2.840 | 3.178 |
| int / 16 位正负 | 36.387 | 5.202 | 3.083 |
| int / 全位宽 | 41.673 | 3.314 | 3.968 |
| int / 16 种值 | 14.296 | 1.902 | 2.526 |
| int / 逆序 | 5.553 | 7.825 | 0.388 |
| long long / 22 位非负 | 41.870 | 4.009 | 3.144 |
| long long / 16 位正负 | 36.489 | 14.084 | 3.021 |
| long long / 全位宽 | 41.590 | 6.616 | 6.614 |
| long long / 16 种值 | 14.489 | 2.917 | 2.483 |
| long long / 逆序 | 5.574 | 9.120 | 0.402 |

含负数的小值域 long long 由原来扫描完整位宽，变为只处理实际 16 位范围，收益明显。逆序输入直接翻转也更快。随机全位宽 int 和只有少量不同值的 int，原专用实现更快，故不将此次抽取宣称为普遍的速度提升。

另一次位宽筛选比较了 8/11/16 位方案；16 位桶表的初始化及缓存成本在这些数据中通常不合算，因此正式分派只保留 8/11 位。筛选记录是原型测量，并非最终共享接口的跑分；[digitCandidates.csv](digitCandidates.csv) 与 [digitCandidates.cpp](digitCandidates.cpp) 保留参数选择依据。

TreeMap 五者评测重新运行 N=Q=100 万、离线候选 U=200 万。当前 TreeMapOff 完整构建中位数 **117.616 ms**，包含候选复制、排序去重、数组初始化和随后逐个插入；完整结果见 [主报告](../../PERFORMANCE.md)。未用两次独立运行之间的差异计算此次排序抽取的优化百分比。

## 验证与复现

TreeMap 和独立排序各通过 GCC GNU++17、GCC 严格 C++17、Clang 严格 C++17、Clang ASan/UBSan 四组检查。测试覆盖 8/16/32/64 位正负整数及极值、升降序、分派边界、跳过位段、透明/自定义比较器、复制/移动、原存储不变，以及 vector/array/deque/string/vector<bool>/list/forward_list。

[原始样本](adaptiveCandidates.csv)、[汇总](summary.csv)、[逐组校验日志](adaptiveCandidates.log)、[源文件指纹](sourceSha256.txt) 一并保留。独立实测复现：

```bash
cd tree_map_experiments
g++-15 -std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic \
  final_results/adaptiveSorting/adaptiveCandidates.cpp -o /tmp/treeMapSortBench
/tmp/treeMapSortBench > /tmp/treeMapSortBench.csv
```

该程序内嵌历史专用 8 位函数，历史共享实现直接包含归档的 legacy/sort.hpp。归档时仅删除未实例化的原型函数并调整头文件相对路径，不改变测量逻辑；重新编译也已验证。

[restorePreviousTreeMap.patch](restorePreviousTreeMap.patch) 可在临时目录内，将本次指纹对应的 592 行头文件恢复为 633 行历史版本；供旧构建报告的候选生成脚本使用，不需覆盖正式文件。当前 581 行版本需先应用 [接口精简的恢复补丁](../apiSimplification/restorePreviousTreeMap.patch) 回到 592 行，再应用本目录补丁。历史版本 SHA-256 记录在本目录指纹文件末尾。
