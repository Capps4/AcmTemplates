# Geo2 数值与性能优化验证

2026-10-02，macOS arm64。保持一个模板参数 T 和原有公开接口。此报告保存上一轮数值优化的快照；当前实现与数据见 [Validation.md](Validation.md)。本快照对应 NumericValidation.json 的源码指纹，命令、测试输出、源码 SHA-256 在 [NumericValidation.json](NumericValidation.json)。仅运行 Geo2、Point、FloatPointNumber 的专项检查，没有启动全库测试。

## 修复与实现

1. **整数中间溢出**：Point 的 dot/cross 乘积、orient 的坐标差和乘积、圆/直线相交的四次乘积、Polygon/Convex 的面积累加在内部使用 __int128_t，返回类型仍为 T。整数线段的位置判定使用方向和包围盒，避免计算仅用于判断符号的大点积。凸边界方向通过第一个非零转向识别，规范化不再要求整个面积放进 T。
2. **大数相消**：运行时浮点叉积用两次 std::fma 补偿乘积误差；FloatPointNumber 增加对应转发。C++17 constexpr 沿用原表达式，GCC/Clang 的常量求值检测保证原接口继续可用。圆/线段判别式改用几何叉积形式和补偿乘积差，二次方程用稳定根公式。
3. **半平面退化与近乎平行**：相反方向的重合约束直接裁切原直线的参数区间，避免反复把舍入交点重新当作边界。整数值浮点输入在范围内时，队列对交点的侧判定先做精确整数运算，再构造坐标。退化点不会再次经过容易将其删空的浮点裁切；另有精确可行性核对。
4. **减少工作量**：凸包/直线只判交集时只查两个极值；最近点复用同次查询结果。凸包/凸包判定和最近点直接消费 Minkowski 边合并结果，不构造临时点数组，额外存储 O(1)；发现公共点时停止遍历。卡壳推进用一条边叉积/点积替代两次投影比较。内部循环索引避免通用取模；半平面队列使用预留 vector 和头下标；圆盘面积对完全在盘内的边跳过开方和反三角函数。

这轮未增加公开类型、模板策略或几何 EPS。对象/基本交互层 1096 行，组合算法层 357 行，共 1453 行；优化前为 1328 行。新增代码主要用于半平面数值与退化处理。注释为英文。

## 正确性检查

- Clang 17：C++17、O2、Wall/Wextra/Werror/pedantic。
- GCC 15.2：GNU++17、O2、Wall/Wextra/Werror/Weffc++。Point 的二维/三维兼容回归去掉 Weffc++，原因是未改动的 Geo3 默认构造有既存的成员初始化告警；其余告警仍作为错误。
- 主测试和范围测试分别在两个编译器下运行四个种子：20261001、7、42、20261002。
- Clang ASan/UBSan 覆盖主测试、范围测试、类型矩阵、半平面 oracle，以及 Point 和 FloatPointNumber 回归；无 sanitizer 报错。
- 共 42 次最终测试执行通过。另有六项负向编译检查，确认整数坐标构造仍被禁止，private 辅助和已移除的接口不能调用。

新增范围测试**每个种子**包含：

| 数据 | 独立参考与数量 |
| --- | --- |
| 坐标范围 1 / 10^3 / 10^6 / 10^9 | 250000 轮方向、线段、圆/线段、圆/直线，独立 __int128 标量公式 |
| 空、单点、线段、一般凸包 | 12000 轮，与 Jarvis 凸包、全边扫描、全点对距离、全部顶点和再建凸包对拍 |
| 浮点整数值输入 | 4000 轮 × double/包装浮点，对照整数位置和直线判定 |
| 大数相消 | 20000 次浮点方向判断，约 10^18 的乘积相减后得到 -1；另有单个整数乘积超过 i64、最终点积/叉积只有 ±4×10^9 的固定用例 |
| 构造坐标 | double/包装浮点共 16000 组，覆盖最近点归属、圆/线段交点、平移后的共边凸包和退化半平面 |
| 螺旋多边形 | 50 顶点的简单细条，最终 area2 = -143599999616，但有符号部分和超过 i64；独立检查非邻边不相交、精确面积和部分和。优化前 UBSan 报整数溢出，最终通过 |

主测试继续覆盖 6561 个格点线段对、圆周/区域最近点、三种标量随机凸包、6000 个独立解析面积参考、2000 组半平面、各类裁切/构造、65536 顶点凸包及整数卡壳等。类型测试覆盖 native/wrapped double/long double 的 36 种 near 组合，以及整数 36 种 inter 判定组合。Point、FloatPointNumber 各自的 100000 轮数学参考与 IO/跨翻译单元测试通过。

半平面另有 Python Fraction 精确裁切参考：四个种子各 1200 例，共 **4800 个不同输入**，每例在 double 和包装浮点两种类型、两个编译器下运行。覆盖重复/相反方向、矛盾约束、共点、共线、交集为有理数单点、几乎平行且叉积为 ±1 的大方向。两个已复现故障永久保存在 [HalfPlaneRegression.txt](HalfPlaneRegression.txt)。

空集判断必须与 Fraction 完全一致。构造结果检查双向边界距离不超过 2×10^-5；面积误差限定为 `10^-7 + 10^-12*参考面积 + 2×10^-5*(参考周长+结果周长)`。薄区域中极小坐标舍入会放大相对面积误差，因此面积容限随周长而变，同时保留双向边界检查，不能靠面积接近来掩盖丢角或多出顶点。范围测试的距离参考容限为 `2×10^-6 + 10^-12*距离`，点归属检查为 2×10^-6。

这些容限仅属于测试。本机 long double 与 double 的精度相同，不作为额外高精度依据；整数及 Fraction 参考才提供独立精确判断。

## 性能测量

O3、NDEBUG，输入与查询预先生成，校验和防止消除计算。测试按串行执行，优化前后在中间轮逆序运行；三个完整运行，每个自测行取 5 次样本中位数、KACTL 行取 7 次，最终再取三轮中位数。原始输出及构建参数见 [BenchmarkSamples.json](BenchmarkSamples.json)。平台、编译器和分布影响绝对耗时及比值。

下面是 **65536 顶点**的优化前 → 后批次耗时，单位毫秒，括号为前/后倍数；小于 1 表示变慢。全部规模见 [BenchmarkBeforeAfter.csv](BenchmarkBeforeAfter.csv)。

| 操作 | Clang 17 | GCC 15.2 |
| --- | --- | --- |
| 直线相交判定，40000 次 | 25.773 → 9.629（2.68×） | 17.086 → 7.651（2.23×） |
| 直线最近点，40000 次 | 22.213 → 10.053（2.21×） | 22.959 → 8.115（2.83×） |
| 外点切线，40000 次 | 5.933 → 5.675（1.05×） | 13.155 → 12.341（1.07×） |
| 三项旋转卡壳，20 次 | 52.347 → 32.724（1.60×） | 57.225 → 33.175（1.72×） |
| 凸包最近点，20 次 | 18.874 → 11.478（1.64×） | 19.949 → 12.719（1.57×） |
| 凸包相交判定，20 次 | 25.055 → 11.430（2.19×） | 26.135 → 12.325（2.12×） |
| Minkowski 和及面积，20 次 | 26.189 → 19.092（1.37×） | 27.739 → 19.440（1.43×） |
| 凸包交集及面积，20 次 | 50.127 → 53.458（0.94×） | 64.397 → 61.715（1.04×） |
| 圆盘交叠面积，20 次 | 8.749 → 6.530（1.34×） | 8.413 → 5.884（1.43×） |

[KACTL 源码快照](References/README.md) 保留原文件并记录许可。LineHullIntersection 原接口返回边下标，Geo2 这里只返回 bool，计算量不同；所以另外提供只查两次 extrVertex 的同结果判定对照。直径同时测原接口和仅将输入从按值改成 const 引用的算法版本，避免把复制成本混为算法收益。最近点对照为基于 extrVertex 的相同投影任务。线段对照返回 vector，Geo2 返回固定两个点；避免分配属于这里的接口性能收益。

| 对照任务与规模 | KACTL / Geo2，Clang | KACTL / Geo2，GCC |
| --- | --- | --- |
| 直线判交，对比原 lineHull，65536 | 3.49× | 2.75× |
| 直线判交，对比同结果 extrVertex，65536 | 2.34× | 1.89× |
| 不相交直线最近点，65536 | 1.58× | 1.94× |
| 直径，对比原按值接口，60001 | 1.42× | 1.77× |
| 直径，对比引用算法，60001 | 1.34× | 1.70× |
| 线段交集，40000 组混合退化输入 | 9.71× | 16.19× |
| 直线判交，同结果算法，1024 | 0.84× | 0.92× |
| 不相交直线最近点，1024 | 0.50× | 0.77× |

完整 KACTL 对照见 [上一轮原始对照输出](BenchmarkSamples.json)。不能据此宣称全部操作胜过所有竞赛模板：小凸包的上述两个查询仍慢于精简 KACTL 版本；凸包交集的 Clang 数据也有回退。保留数值修复、统一实现和简单代码，没有加入按规模切换的多套算法来追逐单项基准。

## 数值范围与剩余限制

基础输入坐标绝对值、圆心和半径不超过 10^9。坐标差最多 2×10^9，单次平方距离、点积和叉积最终绝对值最多 8×10^18，可放进 i64；圆/直线判定的四次乘积用 __int128。面积累加避免部分和先溢出再相消。公开数值结果仍需放进 T；Minkowski 构造扩大坐标后，后续计算必须重新检查范围。加宽是内部实现，不恢复 Value/Wide 参数或返回策略。

半平面整数值路径要求点坐标绝对值 ≤10^9，方向分量绝对值 ≤2×10^9。队列侧判定的两个二次量乘积相加的保守界为 1.28×10^38，小于有符号 128 位上界约 1.70×10^38。这里保证的是这些关键侧判定，不是任意浮点输入的完整精确拓扑；角度排序、参数区间和最终坐标仍使用 T。

double 仍有舍入误差；包装浮点只提供比较容差，不能恢复丢失信息。两次 fma 主要改善乘积相消，不等同于 [Shewchuk 完整鲁棒谓词](https://www.cs.cmu.edu/~quake/robust.html)。极薄区域或极接近平行时最终坐标/面积的相对误差仍可能明显。

常规半平面队列线性扫描，预排序入口 O(n log n+b)，凸包交集常规 O(n+m)。相反重合约束的降维走线性区间路径；其他退化回退仍保留逐次裁切，最坏二次复杂度，没有声称全部退化都已做到线性。

## 复现

在仓库根目录执行：

```sh
clang++ -std=c++17 -O2 -Wall -Wextra -Werror -pedantic template/Geo2/Test.cpp -o /tmp/geo2-test
clang++ -std=c++17 -O2 -Wall -Wextra -Werror -pedantic template/Geo2/TestCaseBounds.cpp -o /tmp/geo2-bounds
TEST_SEED=20261001 /tmp/geo2-test
TEST_SEED=20261001 /tmp/geo2-bounds
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
clang++ -std=c++17 -O3 -DNDEBUG template/Geo2/BenchmarkCompare.cpp -o /tmp/geo2-compare
/tmp/geo2-compare
```

其他种子为 7、42、20261002；GCC 替换为 `/opt/homebrew/bin/g++-15 -std=gnu++17`。完整构建参数和 Point/FloatPointNumber 的测试命令已记录在 JSON；Point/Test.cpp 需要调用方提供 i64 别名，FloatPointNumber/Test.cpp 与 TestExtra.cpp 联合构建。

范围复查：实现只涉及 Geo2、Point 的数值运算和 FloatPointNumber 的 fma 转发，以及对应测试、基准、参考快照和说明。Geo3、Original、历史 snippets、Manifest 保持原样。本轮未拆分更多模块，没有 commit 或 push。
