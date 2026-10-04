# TreeMapOff 复用 ListHelper 排序

已删除 TreeMap 内部的 64 行排序函数。正式头文件从 644 行减到 591 行，包含工程的 ListHelperAdaptive.hpp，候选 vector 通过移动进入 sorted 管道。没有修改 VS Code snippet 或 ListHelper。

展开 VS Code 的 ListHelper snippet（处理 snippet 转义、去掉 $0）后，与工程 ListHelperAdaptive.hpp 的实现完全一致。ListHelper.hpp 是另一份只使用 std::sort 的旧版，本次没有用它替代自适应版。

## 接入方式

```cpp
keys = std::move(keys) | seq::sorted();
```

`keys | sorted()` 会复制输入，移动写法复用输入存储。排序仍可能分配自己的基数缓冲，不能理解为完全零分配。

ListHelper 的基数路径只识别 std::less<>，TreeMapOff 默认的 std::less<Key> 直接传入会走 std::sort。正式实现对标准整数 less/greater 比较作少量适配：升序使用 sorted()，降序在升序排序后反转；其他类型、自定义比较器按原样传入 sorted(compare)。TreeMap 不再实现自己的排序算法，保留候选去重以及 Fenwick 初始化。

## 测量结果

macOS arm64，GCC 15.2，GNU++17，-O2 -DNDEBUG。同进程串行比较，每组预热一次、正式测 5 轮，随机打乱方法顺序。单位 ms，取中位数。

**候选空间构造**包含输入 vector 复制、排序、去重以及值、存在标记、Fenwick 数组初始化；结果仍是空 Map，不包含逐个插入活跃键。每次计时后检查空状态，并插入最多约 4096 个候选样本验证排名、选择和 false 值的存在性。

| 输入 | 原内部排序 | 共享排序，移动并适配 | 直接传原 typed 比较器 |
| --- | ---: | ---: | ---: |
| 100 万 int，22 位随机值域 | 4.504 | 5.318 | 45.116 |
| 200 万 int，22 位随机值域 | 9.975 | 11.574 | 91.696 |
| 100 万 int，全位宽随机 | 4.307 | 5.087 | 41.077 |
| 100 万 int，16 种值 | 3.095 | 3.014 | 14.611 |
| 100 万 long long，全位宽随机 | 7.748 | 9.095 | 40.746 |

共享版的随机整数候选构造在本次测量中慢约 16%–18%；少量重复值及有序输入接近原版。原版能在 8/11 位间选择、跳过恒定桶；ListHelper 固定 8 位且不跳过恒定桶，因此复用并不意味着排序一定更快。基数轮数、散写模式和代码布局也不同，不能把差异全部归因于管道调用。

单独排序时，输入准备在计时外，检查完整输出与 std::sort 的结果一致，包含全部重复值：

| 输入 | 原内部排序 | 移动管道并适配 | 左值管道并适配 | std::sort |
| --- | ---: | ---: | ---: | ---: |
| 100 万随机 int | 3.025 | 3.762 | 3.958 | 39.996 |
| 200 万随机 int | 6.321 | 7.837 | 8.421 | 84.679 |
| 100 万已升序 int | 0.235 | 0.238 | 0.422 | 7.272 |

原始数据见 [raw.csv](raw.csv)，所有 64 组的中位数和单轮范围见 [summary.csv](summary.csv)。共 320 个正式样本及 64 次预热通过检查；排序完整比较 240 次，候选构造和样本查询验证 144 次。覆盖 100/200 万元素、升降序比较、输入升序/倒序、少量重复值、22 位及全位宽整数。桌面环境未隔离，不保证其他机器上具有相同比例。

## 验证与复现

正式接入版本通过 GCC GNU++17 和 Clang 严格 C++17 ASan/UBSan 回归检查，使用 -Werror 编译，无警告。覆盖两种后端、8/16/32/64 位整数极值、bool、字符串、比较器等价键、复制/移动及容量复用；共享排序还检查长度分界点、字节边界和全部重复值。原五方基准程序编译通过，此次没有重新跑五方完整基准。

源文件指纹见 [sourceSha256.json](sourceSha256.json)，编译环境见 [environment.txt](environment.txt)。比较程序通过独立命名空间同时实例化原版、直接替换版和适配版；旧排序由恢复补丁在临时目录生成。

```bash
cd tree_map_experiments
comparisonDir=$(python3 final_results/listHelper/prepareComparison.py)
g++-15 -std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror \
  "$comparisonDir/compare.cpp" -o "$comparisonDir/compare"
"$comparisonDir/compare" > "$comparisonDir/raw.csv"
```
