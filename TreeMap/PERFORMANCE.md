# 强制容量试验：百万规模五方 GNU++17 评测（已回退）

2026-09-30 13:26:50（北京时间）启动的实测，使用当时的 Final.hpp（572 行）及当时的共享 [sort.hpp](final_results/adaptiveSorting/legacy/sort.hpp)（123 行）。TreeMap 实现集中在 `_treemap`，仅导出 TreeMap 与 TreeMapOff；均为 `<int, bool>`，对照为 `std::set<int>`、`std::map<int, bool>` 和 GNU PBDS 的 `tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update>`。

本轮曾将在线 TreeMap 改为必须显式指定容量，删除自动倍增与模式标记，保留显式 reserve。百万 int/bool 的同进程测量未发现预分配相对自动增长的稳定速度优势，接口调整的主要收益是容量边界明确和代码精简，见 [容量报告](final_results/capacity/README.md)。`()` 仍返回 C++17 的 std::optional<Value>，int Node 仍为 16B；此前接口与编译期方向精简见 [历史报告](final_results/apiSimplification/README.md)。共享排序算法保持不变，实验及局限见 [排序报告](final_results/adaptiveSorting/README.md)。此前的 [构建优化](final_results/buildOptimization/README.md)、[GNU++17 清理优化](final_results/gnu17/README.md) 和 [诊断](final_results/diagnostics/README.md) 保留为历史记录。

正式实现已恢复自动扩容，并将候选排序改为复用 ListHelper，保留可选的显式容量构造。下表及 final_results 根目录的数据、日志和指纹保留为此次强制容量试验记录，未冒充回退后的新测量；回退版本此前的五方数据见 [原始数据](final_results/capacity/previousFiveWayRaw.csv) 与 [汇总](final_results/capacity/previousFiveWaySummary.csv)。

ListHelper 接入后的排序和候选空间构造局部测量见 [复用报告](final_results/listHelper/README.md)，本五方表未重新计时。

## 结果

单位 **ns/次操作，越小越快**，每格为 5 轮中位数。构建、遍历以 N 个元素归一化；混合增删按 2Q 次操作计，含排名的混合操作按 4Q 次计。

| 项目 | TreeMap | TreeMapOff | std::set | std::map | PBDS |
| --- | ---: | ---: | ---: | ---: | ---: |
| 构建（含分配/离散化） | 204.19 | 113.78 | 165.13 | 272.61 | 310.16 |
| 查询命中 | 180.01 | 83.25 | 372.85 | 375.40 | 378.90 |
| 查询未命中 | 194.91 | 82.43 | 332.17 | 362.53 | 341.51 |
| 随机删除 | 188.80 | 98.42 | 403.12 | 395.94 | 508.76 |
| 混合增删 | 267.11 | 109.69 | 364.05 | 465.74 | 638.00 |
| 正向遍历 | 7.92 | 1.13 | 64.85 | 65.91 | 65.15 |
| rankOf | 85.41 | 102.29 | N/A | N/A | 167.15 |
| keyAt | 208.14 | 53.93 | N/A | N/A | 394.49 |
| 增删 + rankOf + keyAt | 230.93 | 94.44 | N/A | N/A | 519.28 |

本轮 TreeMap 混合增删约为 std::set 的 1.36 倍速度，TreeMapOff 约 3.32 倍。对 PBDS 的增删/排名混合负载，TreeMap 约 2.25 倍、TreeMapOff 约 5.50 倍速度。TreeMapOff 的完整构建中位数为 113.785 ms；其中包含 200 万候选的复制、排序去重和随后 100 万次插入。

这张表不包含 clear；clear 的改进此前通过单独的前后对照测量，TreeMap 对符合类型条件的 Value 已为 O(1)，候选与活跃键均百万的 TreeMapOff<bool> 清空中位数从 1.609 ms 降为 0.020 ms。逐个删除的前后差异较小且并非全部改善，详见链接报告。

倍数为“对照中位耗时 / 当前中位耗时”。**桌面环境未隔离，不能把跨轮整体跑分差异直接当作代码优化收益**。全部 39 个受支持项目的中位数、最小/最大值及对照倍数保存在 [summary.csv](final_results/summary.csv)，所有单轮数据见 [raw.csv](final_results/raw.csv)。

## 测量条件

- macOS Darwin 24.6.0，arm64；GCC 15.2.0、libstdc++。
- 编译：`-std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic`。
- N = 1,000,000 个初始活跃键，Q = 1,000,000，随机种子 20260929。
- TreeMap 使用固定 N 槽节点池；TreeMapOff 的候选空间 U = 2N = 2,000,000。
- 每项、每容器预热一次，然后测 5 轮；每轮打乱容器顺序，串行运行。
- 构建项包含容量分配，以及离线候选 vector 的复制、排序去重；当前 int 候选经共享排序分派使用 11 位基数排序。其他项目的初始化均在计时外；容器析构、末态校验也在计时外。
- 查询项目使用 contains，未测量 `()` 返回 optional 时的 Value 复制成本。
- 所有结果流都参与计时区间内的相同哈希计算，防止结果被优化消除。遍历项统一消费键；映射值的校验在计时外。

详细环境及参数见 [environment.txt](final_results/environment.txt)。

## 数据与负载

初始键为 `[0, 2N)` 中的 N 个偶数，按随机顺序插入；离线候选包含 `[0, 4N)` 中的 2N 个偶数，并随机打乱后交给构造函数。

| 项目 | 执行内容 |
| --- | --- |
| build | 从空容器依次插入 N 个不同键 |
| containsHit | Q 次均匀抽样查询初始活跃键 |
| containsMiss | Q 次查询 `[0, 2N)` 中的奇数，全部未命中 |
| erase | 按另一随机顺序删除全部 N 个键，均成功 |
| updateMixed | Q 组“删除旧键、插入新键”，共 2Q 次操作，组间保持 N 个活跃键 |
| iterate | 对初始状态进行一次完整正向遍历，共 N 个输出键 |
| rankOf | Q 次查询，参数均匀取自 `[-1, 4N+1]`；约一半大于初始最大键 |
| keyAt | Q 次查询，序号均匀取自 `[0, N)` |
| orderMixed | Q 组“删除、插入、rankOf、keyAt”，共 4Q 次操作 |

更新流将同一槽位的键在 `2i` 与 `2(N+i)` 之间切换，保证每次删除存在、每次插入不存在；节点回收会被持续使用。映射值按键确定，包含 true 和 false，存在性始终用 contains/find 判断。

离线遍历扫描整个 U。此项初始状态的活跃坐标集中在前半段，所以特别容易顺序扫描；不能把这里的遍历倍数直接外推到任意稀疏候选空间。当前只测试随机唯一 int 键及 bool 值，不代表有序插入、大对象或所有竞赛题型。

`std::set` 和 `std::map` 没有对数复杂度的 rank/kth 接口，因此 rankOf、keyAt、orderMixed 标为 N/A，没有用线性扫描模拟参与比较。

## 正确性与复现

GCC 15.2 的 GNU++17 与严格 C++17、Apple Clang 17 的严格 C++17，以及 Clang ASan/UBSan 检查均通过。检查涵盖两种后端、比较器等价键、bool 代理、边界、容量复用、复制/移动和正向迭代，并验证必须显式容量、负/零/满容量、显式 reserve 与移动后源容器的容量限制；保留删除/clear 后 `[]` 默认值、即时资源释放及可观察构造/赋值副作用的回归；新增 optional 的存在/缺失、false 值、独立副本、生命周期、嵌套 optional 与缺失不默认构造的覆盖。排序模块也独立通过相同四组编译/检查，覆盖整数端点、宽度、升降序、分派边界、跳过位段、通用比较器回退、不同容器、复制/移动和原地存储不变。

基准使用独立的线段树生成排名/选择预期结果；**39 次预热和 195 次正式执行全部通过**结果流哈希、完整有序键、元素数和映射值校验。原始与汇总 CSV 的样本数、中位数、最小/最大值和跨容器结果哈希也已核对。

```bash
cd tree_map_experiments
bash runBenchmark.sh
```

脚本会重新进行正确性检查并生成 `final_results/`。需要 GCC（含 GNU PBDS）及 Clang；默认命令为 `g++-15` 和 `clang++`，可用 `GCC_CXX` / `CLANG_CXX` 覆盖。N、Q、轮数和种子可用 `BENCH_N`、`BENCH_Q`、`BENCH_ROUNDS`、`BENCH_SEED` 覆盖。

本次 [运行日志](final_results/run.log)、[GCC GNU++17 检查](final_results/checkGcc.txt)、[GCC 严格 C++17 检查](final_results/checkGccStrict.txt)、[Clang 检查](final_results/checkClang.txt)、[ASan/UBSan 检查](final_results/checkSanitized.txt)、四份独立排序检查日志（`sortCheck*.txt`）与 [源码 SHA-256](final_results/sourceSha256.txt) 一并保留。指纹覆盖两份正式头文件、两份检查程序、基准程序及运行脚本；候选实现只保存实验代码或差异补丁。
