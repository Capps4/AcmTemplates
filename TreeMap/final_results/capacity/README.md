# TreeMap 容量：显式预分配与自动扩容

2026-09-30，macOS arm64 / GCC 15.2，`-std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic`。数据为 100 万个不同 int 键，Value=bool；以下均为同进程、串行、随机顺序测量，预热一次、测 7 轮取中位数。桌面环境未隔离。

## 容量是否带来明显速度差距

专项构建测量使用同一个 run 函数，在计时内根据模式选择固定容量构造、自动增长构造，或者默认构造后 reserve。初始化输入和完整输出检查在计时外。随机与升序输入分别测量，总耗时包含容量分配和全部插入。

单位 ms：

| 百万键的插入顺序 | 提前提供容量 | 自动扩容 | 默认构造后 reserve |
| --- | ---: | ---: | ---: |
| 随机 | 194.386 | 193.866 | 201.475 |
| 升序 | 140.576 | 140.698 | 142.102 |

随机输入下自动扩容比预分配低约 0.27%，升序下高约 0.09%，均没有显示稳定的预分配优势。预分配构造阶段本身中位数约 0.42 ms；自动模式把分配和搬运移入插入阶段。reserve 与容量构造准备的槽位数相同，本次小幅差异不能归因于不同的树算法。各单轮范围见 [focusedSummary.csv](focusedSummary.csv)，全部 42 个正式样本及 6 次预热通过成功插入数、元素数、完整有序键和值的检查。

这说明此类小 Key/Value 的 SBT 构建主要花在查找路径与插入平衡，而不是连续数组扩容。自动增长的额外搬运为线性规模，树插入还有路径与平衡维护成本；本组百万容量最终增长到 1,048,576，和实际元素数很接近。其他规模可能留下更多闲置槽位，大 Key/Value 的构造或搬运成本也可能更高，不能把本组差异当作所有类型和机器的固定比例。

## 原四模式对照

另复用正式五方基准进行四模式对照，包含相同的结果流哈希与独立末态 oracle。查询、混合增删的初始构造在计时外；构建包含容量分配。这里的循环和输入与专项构建计时略有不同，只比较本表内部的结果，不混用两个实验的绝对数值。

单位 ns/实际操作：

| 项目 | 原固定容量 | 原自动增长 | 原 reserve | 删除自动模式后 |
| --- | ---: | ---: | ---: | ---: |
| 构建 | 198.761 | 199.343 | 191.540 | 191.192 |
| contains 命中 | 149.466 | 157.673 | 177.489 | 159.662 |
| 混合增删 | 217.381 | 223.659 | 220.663 | 223.936 |

构建的自动与固定差约 0.3%。删除自动模式的候选在本表构建较快，查询与混合操作较慢；代码布局、内存地址和桌面波动都可能影响计时，因此不将此次接口精简宣称为各操作的普遍提速。84 个正式样本及 12 次预热全部通过结果流、完整键/值和元素数检查，单轮数据见 [raw.csv](raw.csv)。

## 最终接口（已回退强制容量）

未发现预分配相对自动扩容的稳定速度优势，因此恢复原有的两种构造方式：

```cpp
TreeMap<int, bool> map;             // 按需自动扩容
TreeMap<int, bool> fixed(maxAlive); // 显式固定容量
```

默认或仅比较器构造的容器按倍增扩容，也可提前 reserve(n)。显式容量构造仍限制最大同时存活的不同键数，满时新增键抛 length_error；删除槽复用，reserve 可显式增加容量。移动后的源容器恢复为空的自动扩容模式。

头文件恢复为 581 行，保留此前的 optional 读取、编译期方向优化和 16B int Node；TreeMapOff 未改动。以下测量与日志保留为容量试验记录，不代表回退后重新测量。

## 验证与复现

试验期间曾将调用方改为显式容量，并检查无默认构造/无仅比较器构造；此次回退已恢复原调用方式及自动扩容回归用例。试验检查保留容量 0、负容量、满容量、重复键、freelist 复用、显式 reserve 和移动后源容器使用；副本生命周期测试通过显式 reserve 触发扩容。

TreeMap 与排序各通过 GCC GNU++17、GCC 严格 C++17、Clang 严格 C++17 和 Clang ASan/UBSan。正式百万规模五者评测也重新运行，39 次预热和 195 次测量通过独立结果流与完整末态检查，见 [主报告](../../PERFORMANCE.md)。前一次完整五者数据以 previousFiveWay 开头的文件保留。

[focused.csv](focused.csv) / [focused.log](focused.log) 保留专项构建数据；[raw.csv](raw.csv) / [run.log](run.log) 保留四模式对照。复现脚本校验正式源文件指纹，根据输入指纹判断 581 行自动扩容版或 572 行强制容量试验版，通过 [恢复补丁](restorePreviousTreeMap.patch) 正向或反向生成另一版，并将强制容量试验版放入独立命名空间。两者共享同一个 sort.hpp。基准 core 仅为改名的 main 补 return 0，并在计时外将结果流索引改成 at 以避免 GCC 对专用 harness 的边界警告。

```bash
cd tree_map_experiments
comparisonDir=$(python3 final_results/capacity/prepareComparison.py)
g++-15 -std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic \
  "$comparisonDir/capacity.cpp" -o "$comparisonDir/capacity"
"$comparisonDir/capacity" > "$comparisonDir/raw.csv"
g++-15 -std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic \
  "$comparisonDir/focused.cpp" -o "$comparisonDir/focused"
"$comparisonDir/focused" > "$comparisonDir/focused.csv"
```

脚本恢复出的历史头文件指纹及两份程序的编译已核验。正式实现已恢复为 581 行，排序现已内置；历史复现脚本需先恢复其要求的源码指纹，归档的独立排序头文件见 [legacy](../adaptiveSorting/legacy/)。
