# C++17 接口与实现精简

此前已回退强制容量；本报告对应排序尚独立时的 581 行版本；容量试验与回退说明见 [容量报告](../capacity/README.md)，当前排序已内置，复现脚本需与本实验的历史源码指纹匹配。

2026-09-30，GCC 15.2 / macOS arm64。正式 TreeMap 头文件从 592 行减到 581 行，排序头文件保持 123 行；int Key 的热节点仍为 16B，Value 继续存于 sidecar vector。

## 保留的改动

- `operator()` 返回 `std::optional<Value>`：缺失返回 nullopt，不插入、不默认构造 Value；命中复制 Value。删掉共享默认值的静态 vector。bool 值为 false 时 optional 仍有值，`if (map(key))` 判断存在，`*value` 读取实际值。
- 旋转合为 `rotate<bool Right>()`，方向在编译期确定，保留 size 转移和一次 pull。
- 插入的左右回溯合到泛型 lambda，通过 std::false_type / std::true_type 类型标签确定 constexpr 方向，保持比较顺序和 directional maintain。递归前不持有节点字段引用，扩容不会使局部引用悬空。
- Fenwick 更新循环使用 u32，删除唯一的 u64 别名。候选容量最多 INT_MAX，更新最后一步下标至多到 2^31，u32 足够。

`()` 返回副本，不改变 Map，但副本自身可修改，且不受随后扩容/删除/clear 影响。调用此接口要求 Value 可复制，大 Value 的读取有复制成本；非复制 Value 的增删、移动、[] 和 const 迭代仍可使用。没有加入 optional<reference_wrapper<...>> 或额外查询接口。

TreePath 只复制有效祖先路径的实现、移动后源容器可继续使用的构造代码均保留。没有增加节点字段、parent、运行时后端分派或 bool 特化。

## 同进程前后对照

N=Q=100 万，离线候选 200 万，`-std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic`。每个项目/版本预热一次，测 5 轮，随机顺序串行执行。基于正式基准复用独立线段树 oracle，输入准备和末态校验在计时外；结果流哈希在计时内。40 个正式样本与 8 次预热全部通过结果流和完整末态检查。

单位 ns/实际操作，中位数：

| 项目 | 原 SBT | 最终 SBT | 原 Off | 最终 Off |
| --- | ---: | ---: | ---: | ---: |
| 构建 | 188.069 | 190.795 | 112.626 | 112.922 |
| 混合增删 | 214.884 | 215.736 | 103.194 | 104.745 |

最终 SBT 构建耗时高约 1.4%，混合增删高约 0.4%；Off 差异约 0.3%/1.5%。桌面环境未隔离，不据此宣称速度提升或跨机器保证。

第一次把方向合为运行时 bool，SBT 构建 190.581 → 208.273 ns/键，慢约 9.3%；即使分别以 true/false 调用 maintain，仍为 188.567 → 199.902，慢约 6.0%。两个版本均排除，正式实现保留类型标签分派。候选原始数据分别见 [runtimeDirection.csv](runtimeDirection.csv) 和 [constantMaintain.csv](constantMaintain.csv)，补丁记录在同目录。

以上负载不调用 `()`，测的是结构操作。optional 接口通过功能检查验证，不将此表解释为大对象复制查询的性能测试。完整百万规模五者评测已重新运行，见 [主性能报告](../../PERFORMANCE.md)。此前五者数据保存在 previousFiveWayRaw/previousFiveWaySummary，供历史核对，不能把跨运行差异全部归因于精简。

## 验证与复现

TreeMap 与独立排序各通过 GCC GNU++17、GCC 严格 C++17、Clang 严格 C++17 和 Clang ASan/UBSan；正式五者评测 39 次预热、195 次正式执行通过独立结果流及完整键/值末态检查。

optional 回归覆盖缺失、存在的默认值、false、bool 原生代理写入、修改返回副本、跨扩容/删除/clear、嵌套 optional 的空 payload、缺失不默认构造和 value_or；非复制 payload 用 [] / const 迭代检查，其余移动、资源释放测试保留。

[raw.csv](raw.csv)、[summary.csv](summary.csv)、[run.log](run.log)、[sourceSha256.txt](sourceSha256.txt) 保留最终同进程对照；正式完整基准的环境、指纹与检查日志见上一层目录。复现：

```bash
cd tree_map_experiments
comparisonDir=$(python3 final_results/apiSimplification/prepareComparison.py)
g++-15 -std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic \
  "$comparisonDir/compare.cpp" -o "$comparisonDir/compare"
"$comparisonDir/compare" > "$comparisonDir/raw.csv"
```

生成脚本检查正式源文件指纹，通过恢复补丁生成历史 592 行版本，再改名到独立命名空间；两版本共享同一个排序实现。复用基准时为改名的 main 补 return 0，结果流数组检查改成 at（在计时外）以避免 GCC 对专用 harness 的边界警告；测量循环未改。脚本恢复出的历史头文件指纹和编译已核验。
