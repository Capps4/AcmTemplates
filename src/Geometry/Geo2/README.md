# Geo2

二维几何分四层头文件，后层包含前层；需要全部功能时 include Circle.hpp。实现位于 _geo2，公开接口导出到全局。

| 头文件 | 内容 |
| --- | --- |
| [PointVec.hpp](PointVec.hpp) | Point/Vec、方向判断、共享求交结果 |
| [SegLine.hpp](SegLine.hpp) | Line/Seg 与点的交互 |
| [PolygonConvex.hpp](PolygonConvex.hpp) | Polygon/Convex、凸包、裁切与半平面交 |
| [Circle.hpp](Circle.hpp) | 圆周、切线、圆间关系与圆盘交叠面积 |

Point 与 Vec 区分；Line 的方向须非零，Seg 可零长度。Polygon 为含边界的简单多边形；Convex 支持空/点/线段，顶点只读且规范为逆时针、无冗余共线点。任意点集用 convexHull，fromBoundary 只规范化已有凸边界。

`loc(p)` 返回 OUT/ON/IN；`inter(a,b)` 构造交集，`inter(a,b,nullptr)` 只判交，`near(a,b)` 返回两个对象上各一个最近点。Hit 的 kind 为 NONE/ONE/TWO/SEG/CO，CO 表示无限交集，不能用 size()==0 判为空。多边形与线的交集返回多个闭线段，凸包交集返回 Convex。

Circle 表示圆周，loc 的 IN 只作位置分类；`intersectionArea` 明确计算圆盘交叠面积。圆与多边形的完整圆弧交集、一般多边形之间的完整区域交集不构造。near 要求对象非空；距离由返回的点计算。

标量 T 选整数、原生浮点或 FloatPointNumber。整数可判交，生成新坐标的交点/投影/最近点/重心要求浮点；公开结果须可由 T 表示。整数坐标绝对值和半径≤1e9 时内部使用加宽运算，组合算法扩大坐标后须重新核对范围。double 使用原生比较，包装浮点使用其容差；接近退化的任意实数输入不保证完全精确。

凸包构建 O(n log n)，凸包点位置/外点切线/与线判交 O(log n)；凸包最近点与线段最坏 O(n)。旋转卡壳 O(n)，Minkowski 及凸包间判交/最近点 O(n+m)。凸包交集常规 O(n+m)、退化回退最坏 O(m(m+n))；半平面交须显式给有限凸域，常规 O(n log n+b)、退化回退最坏 O(n(n+b))。一般多边形间判交/最近点 O(nm)。

KACTL 改写部分遵守本目录 [LICENSE](LICENSE)。二维和三维模板导出的类型同名，组合时自行区分。

[使用示例](../../../tests/Correctness/Geometry/Geo2/Demo.cpp)。
