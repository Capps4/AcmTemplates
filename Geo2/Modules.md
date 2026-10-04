# Geo2 四文件拆分与容器访问

2026-10-04，基于用户完成修改后的浮点数代码重新验证。

## 文件与依赖

| 文件 | 职责 |
| --- | --- |
| PointVec.hpp | Point、Vec，共享结果类型，orient，点/点 inter、near |
| SegLine.hpp | Line、Seg，它们与点的交互 |
| PolygonConvex.hpp | Polygon、Convex，与点/线段/直线的交互，凸包构造及组合算法 |
| Circle.hpp | Circle，与前面所有形状的交互，切线、关系及圆盘交叠面积 |

包含链为 Circle → PolygonConvex → SegLine → PointVec → FloatPointNumber。检查确认前层没有后层头文件依赖；跨形状操作放在更靠后的层。Geo2 仅有这四个实现头文件。Point/Final.hpp 和 GeometryCompatibility/Geo2.hpp 分别保留为 PointVec、Circle 的转发入口。旧 Geo2/Final.hpp、Algorithms.hpp 已移除，调用方需要更新这两个包含路径。

每层通过明确的 using 导出公开类、结果类型和函数。Point、Vec、Line、Seg、Polygon、Convex、Circle 及 convexHull、inter、near 等均可直接使用，不需要命名空间前缀。Coord 及辅助实现留在 _geo2，support、edge 等继续 private。两个对称重载宏由 PointVec 定义，后层复用，Circle 末尾统一 undef。

Point/Vec 的共享实现原样迁入 PointVec。凸包组合算法除一处整数提取调整外原样迁入 PolygonConvex。没有增加面积缓存、顶点缓存或对象字段。

## 容器式访问

Polygon 增加可写和 const 的 operator[]、front/back、begin/end，另有 cbegin/cend、empty。支持范围 for 和标准迭代器算法；增删顶点仍可操作公开 ps。

Convex 原有只读 operator[] 保留，增加 front/back、begin/end、cbegin/cend。非 const 凸包也只能取得 const 顶点引用，避免破坏规范化边界和最低顶点下标；vertices() 保留只读 vector 视图。

```cpp
#include "template/Geo2/PolygonConvex.hpp"

Polygon<Float> polygon{{{0, 0}, {4, 0}, {4, 3}, {0, 3}}};
auto hull = convexHull(polygon.ps);
auto first = hull[0];
for (const auto &p : hull) {
    // Read canonical hull vertices.
}
for (auto &p : polygon) p += Vec<Float>{1, 0};
```

公开继承 std::vector 是合法 C++，但会暴露任意增删顶点的能力，无法维护 Convex 的内部约束。因此保留已有 vector 成员，用简单访问函数转发；存储、访问复杂度和迭代器能力保持一致。

## 浮点数依赖

用户当前 Float = FloatPointNumber<double>，默认输出精度为 10。其 round 采用 EPS 边界、半整数向正无穷方向取整的规则。本轮保留用户最新浮点数实现和测试，在验证开始及结束核对其指纹一致。

半平面交的整数值精确路径不再调用包装类的 round，而是对 val() 调用 std::llround；原生数的路径仍直接转换。只有通过范围和整数值检查后才使用精确整数拓扑。这样几何判定不依赖用户的通用舍入规则，也不引入包装类型到 double 的隐式转换。新增负坐标裁切回归，现有四种子随机半平面交和 Fraction 参考全部通过。

Point 输入输出测试显式设置精度为 6，验证流行为，避免把包装类的默认输出位数当作几何要求。Float 类自身测试继续验证当前默认精度为 10。

## 最终验证

[ModuleValidation.json](ModuleValidation.json) 保存当前源码指纹、命令、种子和逐项结果。最终 144 项检查全部通过：

- 42 次既有专项运行：Geo2 主测试、数值边界、类型矩阵、半平面交、Point、二维/三维兼容入口、最新浮点类测试。
- 15 次新增模块运行：三个构建配置分别验证四层独立包含及三翻译单元链接。覆盖 i64、double、Float、包装 double、long double 和包装 long double。
- 48 项包含顺序检查：两个编译器分别验证四文件的 24 种排列，并重复包含头文件和兼容入口。
- 24 项负向编译检查：整数坐标构造限制、私有接口、只读凸包引用/迭代器、未导出的 Coord，以及前层不能访问后层类型等。
- 1 项直接依赖检查。
- 12 次 README demo 输入输出检查：两个编译器覆盖空输入、单点、重复点、全共线、普通凸包及 ±10⁹ 坐标。
- 2 项参考基准程序编译检查。

三个配置为 Apple Clang 17 C++17 O2 严格告警、GCC 15.2 GNU++17 O2 严格告警及 Clang ASan/UBSan。GCC 的既有 Geo3 兼容测试按此前做法仅移除 Weffc++，其余告警继续作为错误处理；Geo3 实现未修改。

专项包含四种子边界规范化与容量检查、共点/共线/相切/重叠等退化情况、大整数中间乘积及面积相消、4800 组独立 Fraction 半平面交参考，以及 native/wrapped double/long-double 最近点和判交类型矩阵。整数距离与交点构造限制、圆周/区域语义保持原样。

未启动全库测试。double 和包装浮点仍受舍入影响；本次测试并不意味着任意实数输入的完整精确几何。半平面交的退化回退复杂度保持既有说明。

## 性能对照

拆分前后均使用用户最新的同一份 Float 头文件，使用 O3、NDEBUG。基准按串行方式执行三个完整轮次，中间轮次反转顺序；每个操作本身取五次采样的中位数，再取三个完整轮次的中位数。

Clang/GCC 的五个代表性函数在移除源文件元数据、统一局部函数标签编号后生成相同汇编：整数凸包位置、整数凸包/直线判交、整数向量叉积、double 凸包面积和 double 圆盘交叠面积。新增容器接口没有额外存储或虚函数。

65536 顶点组各操作的“旧耗时 / 新耗时”比值中位数：Clang **1.002**，GCC **0.998**。整体接近原性能，个别操作有少量升降，不把这些时序差异解释为算法提升，也不宣称所有操作更快。

Clang，单位 ms；比值越大表示新版本越快：

| 操作 | 拆分前 | 拆分后 | 旧/新 |
| --- | --- | --- | --- |
| boundary_dense | 3.195 | 3.163 | 1.010 |
| boundary_sparse | 2.329 | 2.321 | 1.003 |
| inter_predicate | 7.878 | 7.863 | 1.002 |
| near_line | 8.347 | 8.379 | 0.996 |
| tangents | 5.651 | 5.645 | 1.001 |
| calipers | 31.157 | 30.293 | 1.029 |
| near_convex | 11.453 | 11.54 | 0.992 |
| inter_convex_predicate | 11.66 | 11.567 | 1.008 |
| minkowski | 18.765 | 18.377 | 1.021 |
| convex_intersection | 50.696 | 51.087 | 0.992 |
| circle_polygon_area | 6.546 | 6.568 | 0.997 |

GCC，单位 ms：

| 操作 | 拆分前 | 拆分后 | 旧/新 |
| --- | --- | --- | --- |
| boundary_dense | 3.804 | 3.888 | 0.978 |
| boundary_sparse | 2.711 | 2.702 | 1.003 |
| inter_predicate | 7.399 | 7.395 | 1.001 |
| near_line | 7.964 | 8.098 | 0.983 |
| tangents | 12.233 | 12.735 | 0.961 |
| calipers | 32.876 | 33.709 | 0.975 |
| near_convex | 12.923 | 12.821 | 1.008 |
| inter_convex_predicate | 12.315 | 12.331 | 0.999 |
| minkowski | 21.027 | 21.064 | 0.998 |
| convex_intersection | 58.741 | 60.46 | 0.972 |
| circle_polygon_area | 5.877 | 5.88 | 0.999 |

完整数据保留在 [BenchmarkModules.csv](BenchmarkModules.csv) 和 [BenchmarkModuleSamples.json](BenchmarkModuleSamples.json)，包含 1024、8192、65536 顶点组、原始采样及构建命令。此前朋友/KACTL 对照和数值优化数据作为历史记录保留。

## 范围 Review

修改限于四层几何实现、原入口转发、对应测试/基准的包含路径、容器与全局接口验证，以及相关文档。用户最新 Float 实现与测试、Original、参考代码、Geo3 和 Manifest 未由本轮修改。没有提交或推送。
