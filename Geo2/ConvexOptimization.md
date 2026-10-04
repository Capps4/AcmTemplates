# 凸包构建与极值查询优化

2026-10-02，macOS arm64。按确认的两项方案修改 Convex::fromBoundary、convexHull 和私有 support；面积和周长继续扫描，没有增加缓存。公开接口、单一 T、逆时针/字典序起点的边界规则，以及整数加宽与浮点补偿保持不变。

## 最终实现

- **原地规范化**：读取一个顶点后，在同一数组中维护有效顶点的栈。写入位置不超过已读取位置，不会覆盖未来输入。重复点、方向、首尾共线清理和退化处理保留原有逻辑，没有为了省扫描跳过包装浮点所需的检查。
- **容量处理**：单调链工作数组预留 n+1 个点，替代 2n。规范化后若容量超过有效点数的两倍，复制到紧凑数组并释放旧缓冲区；普通边界不再创建第二个清理数组。空结果释放缓冲区。容量比例是内存占用取舍，不按测试规模切换几何算法。
- **方向起点**：Convex 只新增一个 private int，记录最低、再最左的顶点。字典序最小点开头的逆时针凸边界中，向右且向下的边构成连续前缀，可用 O(log n) 次纯坐标比较找到最低顶点，避免新增一次全数组扫描。
- **极值二分**：最低顶点将边分成两个物理连续的极角有序区间。查询先选区间，再用边方向的半圈标记和叉积二分；循环中省掉相对首边的半圈叉积，也不逐层转换循环索引。零方向和点/线段处理保持原样。没有角度计算、边数组或第二套数值模板。

fromBoundary 仍要求输入已经是有序凸边界，不增加任意点集求凸包或凹多边形验证的职责。算法结果的规范化继续 O(n)；极值查询继续 O(log n)。原地清理的主要确定收益是减少分配和缓冲区占用，时间收益随数据和编译器变化。

实现由 1453 行增加为 1465 行（含空行和英文注释）；新增字段为一个 int。Point、FloatPointNumber、Geo2/Algorithms.hpp 实现与本轮开始时的 SHA-256 一致。没有增加公开辅助方法，没有改动 Original、Geo3、Manifest，也没有 commit/push。

## 正确性验证

[Validation.json](Validation.json) 保存最终源码指纹、构建参数及输出。Clang 17 / GCC 15.2 的四个种子（20261001、7、42、20261002）专项测试，以及 Clang ASan/UBSan 全部通过，共42次最终执行。GCC 的 Geo3 兼容回归仍只去掉既存的 Weffc++ 告警，其余专项保留原来的严格参数。另有六项负向编译检查确认公开接口未扩张。未运行全库测试。

新增边界测试每个种子包含15000组 i64 / double / FloatPointNumber<double> 规范化用例和300000次直线判交查询：

- Jarvis 与 __int128 标量公式作为独立参考。
- 任意起点、顺/逆时针、相邻重复点、重复首点、边上中点、空/单点/线段退化。
- 数据尺度2、2000、2000000、200000000，坐标绝对值不超过10^9。
- 四个轴方向、沿边方向，以及随机方向，覆盖极角的两个有序区间与首尾边。
- 查询原对象以及复制后移动的对象，检查内部下标随值正确复制。
- 人为扩大输入容量，检查最终容量不会保留远大于有效点数的缓冲区。

四个种子共20000组几何边界，在三种类型下合计60000次规范化检查和1200000次新增判交查询；两个编译器分别执行这些输入。原有100万轮整数范围数据、构造点/最近点/圆与线段参考、65536顶点测试、4800个 Fraction 半平面参考和两个固定退化回归继续通过。上一轮数值误差容限与精度限制保持原样，见 [数值优化记录](NumericOptimization.md)。

## 测量方法

本轮开始前冻结实现快照，优化前后使用相同新基准源码、编译参数和输入。O3、NDEBUG；自测与KACTL用C++17，朋友原文件使用span，因此双方朋友对照均用C++20。

输入与查询生成在计时外，先核对结果；三轮串行完整运行，中间轮反转顺序。每行自测取5次、朋友/KACTL取7次样本中位数，最终再取三轮中位数。较小差异可能受频率、缓存和系统调度影响，不宣称几百分比的跨机器收益。

[BenchmarkConvexSamples.json](BenchmarkConvexSamples.json) 保存全部输出、参数和源码指纹；[BenchmarkConvex.csv](BenchmarkConvex.csv) 对比本轮前后，[BenchmarkFriend.csv](BenchmarkFriend.csv) 保存朋友对照。Benchmark.csv 和 BenchmarkComparison.csv 更新为当前版本数据；上一轮 BenchmarkBeforeAfter.csv 与 BenchmarkSamples.json 保留为历史数值优化记录。

下面是旧 → 新耗时，单位毫秒，括号是旧/新倍数，大于1代表本轮优化后更快。

| 自测操作，65536输入/顶点 | Clang 17 | GCC 15.2 |
| --- | --- | --- |
| 稠密凸边界规范化+面积，20次 | 3.146 → 3.135（1.00×） | 4.001 → 3.836（1.04×） |
| 方形边上共线点规范化+面积，20次 | 2.460 → 2.305（1.07×） | 2.656 → 2.733（0.97×） |
| 直线判交，40000次 | 9.778 → 8.071（1.21×） | 7.774 → 7.619（1.02×） |
| 直线最近点，40000次 | 10.035 → 8.435（1.19×） | 8.397 → 8.283（1.01×） |
| Minkowski和+面积，20次 | 19.755 → 18.840（1.05×） | 23.424 → 21.277（1.10×） |
| 凸包交集+面积，20次 | 56.106 → 51.460（1.09×） | 63.639 → 62.970（1.01×） |

| 同朋友输入上的操作 | 规模 | Clang旧 → 新 | GCC旧 → 新 |
| --- | --- | --- | --- |
| 抛物线点集建凸包+取面积，10次 | 60001 | 24.067 → 22.684（1.06×） | 25.064 → 24.282（1.03×） |
| 随机点集建凸包+取面积，10次 | 65536 | 33.139 → 31.824（1.04×） | 33.650 → 32.680（1.03×） |
| 直线判交，40000次 | 60001 | 5.945 → 4.058（1.46×） | 4.821 → 2.545（1.89×） |

在65536个方形边界点中，最终仅四个顶点：两种编译器均测得旧容量65536、新容量4。对于Point<double>，顶点缓冲区从1048576字节降到64字节（不计vector对象及新增下标）。稠密凸边界在两种版本中均需要保存全部顶点。

## 当前与朋友实现的对比

全部整数抛物线顶点及40000组查询在计时前核对，方向选择与原对照保持一致。以下为朋友/当前耗时，大于1代表我们更快；外点切线从朋友两版中选实测更快的一版，并取该行对应的我们耗时。

| 操作，60001顶点 | Clang 17 | GCC 15.2 |
| --- | --- | --- |
| 构建+一次总面积 | 1.11× | 0.88× |
| 点的位置 | 1.86× | 1.15× |
| 外点切线 | 1.64× | 1.33× |
| 直线判交 | 0.82× | 1.34× |
| 直径平方 | 1.57× | 1.73× |

直线判交相较本轮旧版本快约1.46×（Clang）/1.89×（GCC）；相较朋友实现，GCC快约1.34×，Clang仍慢约22%。构建收益较小，GCC构建依然慢于朋友。并非全部项目都胜出：小规模及个别未改动操作的耗时有回退，全部原始行保留，不以选择性数据宣称全面更快。

朋友的line predicate对照是tangentLine查两侧极值后判符号，与我们的bool inter执行相同任务。原文件的构造交点和最小外接矩形在固定正方形上返回错误，仍未计入速度排名。朋友保存面积前缀和，反复查询面积的O(1)优势保留；我们按用户要求继续O(n)扫描。旧对照方法与错误反例见临时目录 /tmp/geo2-friend-compare；原文件未修改。

## 复现

在仓库根目录：

```sh
clang++ -std=c++17 -O2 -Wall -Wextra -Werror -pedantic template/Geo2/TestCaseBounds.cpp -o /tmp/geo2-bounds
TEST_SEED=20261001 /tmp/geo2-bounds
clang++ -std=c++17 -O2 template/Geo2/Test.cpp -o /tmp/geo2-test
TEST_SEED=20261001 /tmp/geo2-test
clang++ -std=c++17 -O2 template/Geo2/TestCaseTypes.cpp -o /tmp/geo2-types
/tmp/geo2-types
python3 template/Geo2/GenerateHalfPlaneCases.py /tmp/geo2-hpi.txt --seed 20261001 --rounds 1200
clang++ -std=c++17 -O2 template/Geo2/TestCaseHalfPlane.cpp -o /tmp/geo2-hpi
/tmp/geo2-hpi /tmp/geo2-hpi.txt
/tmp/geo2-hpi template/Geo2/HalfPlaneRegression.txt
clang++ -std=c++17 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer template/Geo2/TestCaseBounds.cpp -o /tmp/geo2-bounds-san
/tmp/geo2-bounds-san
clang++ -std=c++17 -O3 -DNDEBUG template/Geo2/Benchmark.cpp -o /tmp/geo2-bench
/tmp/geo2-bench
```

GCC替换为 /opt/homebrew/bin/g++-15 -std=gnu++17。其他随机种子为7、42、20261002。朋友前后对照的适配源码与构建命令位于 /tmp/geo2-convex-friend-before.cpp、/tmp/geo2-convex-friend-after.cpp 及 BenchmarkConvexSamples.json；快照位于 /tmp/geo2-convex-before。所有临时路径对应本机文件，临时文件清理后需重新准备快照。
