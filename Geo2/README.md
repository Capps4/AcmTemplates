# Geo2

二维基础几何模板。实现留在 `_geo2`，公开类、结果类型和关键函数通过 `using` 导出到全局，使用时无需命名空间前缀。所有对象只用一个标量参数 `T`：整数用 `i64`，原生浮点用 `double`，需要容差时用全局 `Float`（当前为 `FloatPointNumber<double>`）或显式包装浮点类型。几何层没有 EPS、PI、sgn 或第二套数值策略。

实现集中在四个头文件，后层包含前层，前层不依赖后层：

| 文件 | 内容 |
| --- | --- |
| `PointVec.hpp` | 点、向量、方向判断、共享结果类型、点与点的交互 |
| `SegLine.hpp` | 直线、线段及它们与点的交互 |
| `PolygonConvex.hpp` | 多边形、凸包、与前面形状的交互及凸包基础算法 |
| `Circle.hpp` | 圆、与前面所有形状的交互、切线、圆间关系和交叠面积 |

包含链为 `Circle → PolygonConvex → SegLine → PointVec → FloatPointNumber`。只需要凸包时包含 `PolygonConvex.hpp`；需要全部功能时包含 `Circle.hpp`。旧 `Point` 和 `GeometryCompatibility` 目录已移除，直接包含对应的分层头文件。原 `Geo2/Final.hpp`、`Algorithms.hpp` 也已移除，使用旧路径的调用方需要改包含路径。

`Point、Vec、Seg、Line、Polygon、Convex、Circle`、所有公开结果类型及本页列出的几何函数均可在全局直接使用。每层导出该层完整的 `inter/near` 重载集合；`Coord` 和实现辅助不会导出到全局。

## 只记住三个交互入口

| 接口 | 意义 | 返回值 |
| --- | --- | --- |
| `shape.loc(p)` | 点的位置 | `Location::OUT / ON / IN` |
| `inter(a,b)` | 构造交点或交集 | 按输入类型决定，见下表 |
| `inter(a,b,nullptr)` | 只判断有没有交集 | `bool`，支持整数，无交点坐标构造 |
| `near(a,b)` | 两个对象上的最近点 | `Near<T>`，即 `std::pair<Point<T>,Point<T>>` |

```cpp
#include "template/Geo2/Circle.hpp"
using i64 = long long;

auto h = convexHull<i64>({{0, 0}, {4, 0}, {4, 3}, {0, 3}});
bool inside = h.loc({1, 1}) == Location::IN;
bool crosses = inter(h, Seg<i64>{{-1, 1}, {5, 1}}, nullptr);

using F = FloatPointNumber<double>;
auto region = convexHull<F>({{0, 0}, {4, 0}, {4, 3}, {0, 3}});
auto hit = inter(region, Line<F>({-1, 1}, {5, 1}));
auto [p, q] = near(region, Seg<F>{{6, 0}, {6, 3}});
auto distance = p.dist(q);
```

`near` 的第一个点属于第一个参数，第二个点属于第二个参数；点参数也遵循这一规则。多个最优解允许返回任意一个，交换参数后距离相同，但不保证选到同一组点。相交时返回公共点。空多边形或空凸包没有最近点，调用 `near` 要求对象非空。

距离仅由两个点的 `dist` / `dist2` 计算。形状不再公开 `dist`、`signedDist`、`boundaryDist`、`foot`、`nearest` 或 `contains`。直线投影就是 `near(point,line).second`。

## 对象与语义

- `Point<T>` / `Vec<T>`：实现在 PointVec，点和向量独立，`Point - Point = Vec`。
- `Line<T>`：公开 `p、v`，表示 `p+t*v`。`Line(a,b)` 经过两点；`fromVec(p,v)` 保留给定方向。方向必须非零。自身操作保留 `eval、side、parallel、orthogonal、reflect、reversed`，点位置由 `loc` 判定。
- `Seg<T>`：公开端点 `a、b`，允许零长度；保留 `vec、len2、len、line、mid`。`line()` 要求端点不同。
- `Polygon<T>`：公开有序顶点 `ps`，表示包含边界的简单多边形区域，可凹、可顺时针，不重复首点闭合。支持可写 `operator[]、front/back、begin/end`，const 对象只读，另有 `cbegin/cend、empty`。可用范围 for 和标准迭代器算法；仍可通过 ps 做增删。保留 `size、area2、area、perimeter、isConvex、centroid、hull`。
- `Convex<T>`：表示包含边界的凸区域，支持空、单点、线段。顶点只读，支持 `operator[]、front/back、begin/end、cbegin/cend`，包括非 const 对象的访问也只读；`vertices()` 保留为 const vector 视图。可直接范围 for 遍历。规范为逆时针、字典序最小点开头、无重复和冗余共线点。任意点集用 `convexHull`；已有凸边界用 `fromBoundary`，后者只规范化，不检验任意输入是否凸。规范化在输入数组内清理，容量远大于结果时收缩；内部保存一个最低顶点下标，用于方向极值二分。保留 `size、empty、polygon、area2、area、perimeter、tangents`。
- `Circle<T>`：表示**圆周**，公开 `o、r`，半径非负。零半径表示一个点。`loc` 判断圆周围成区域的内/边界/外；`inter` 和 `near` 按圆周计算。例如圆心的 `loc` 是 IN，但圆心与非零圆周没有交集，到圆周的最近距离是半径。保留 `area、perimeter、circum`；面积是圆周围成区域的面积。

直线和线段的 `loc` 只有 OUT / ON；退化凸包也没有 IN。多边形、凸包的 IN 和 ON 都属于对象；圆的 IN 用于位置分类，不属于圆周对象。

`edge`、循环索引 `at`、`support`、`lineHit`、边界直线排序和有序半平面交均为 private 实现辅助。没有公开内部拓扑下标或辅助结果类型。两个短宏在 PointVec 定义，后层复用它们生成对称重载，Circle 末尾统一 `#undef`。只包含前层时宏仍可供后层使用。

## 支持的求交结果

`inter(a,b,nullptr)` 支持 Point / Line / Seg / Polygon / Convex / Circle 任意组合及参数顺序。`near(a,b)` 也支持这些对象的任意组合，产生新坐标的查询需要浮点 T。

| `inter(a,b)` 输入 | 返回值 |
| --- | --- |
| 点与任意对象 | `Hit<T>`：空或原输入点，整数也可用 |
| 直线/直线 | `Hit<T>`：空、点、重合 |
| 直线/线段、线段/线段 | `Hit<T>`：空、点、重叠线段 |
| 凸包与直线/线段 | `Hit<T>`：空、点、闭线段 |
| 一般多边形与直线/线段 | `vector<Seg<T>>`：多个闭区间，允许孤立零长度线段 |
| 凸包/凸包 | `Convex<T>`：闭交集，包含空/点/线段退化结果 |
| 圆与直线/线段/圆 | `Hit<T>`：空、一个或两个圆周交点、圆周重合 |

`Hit<T>` 公开 `kind、ps`，kind 是 `NONE / ONE / TWO / SEG / CO`，ps 为固定两个点，无堆分配。`size()` 是存储的有效点数；CO 表示无限交集，点数为零。`bool(hit)` 判断存在交集。直线相关交点沿直线方向排序，圆/线段交点沿线段 a→b 排序。

圆周与多边形区域的完整交集可能包含圆弧，本模板暂不构造该结果；两种输入仍支持相交判定与最近点。一般多边形之间、一般多边形与凸包之间的完整区域交集也暂不构造，支持判定与最近点。不用边界交点冒充区域交集。需要圆与多边形**边界**的交点时，可从只读顶点逐条构造 `Seg`，调用 `inter(circle,seg)`。

## 独立几何操作

面积、周长、切线、裁切和组合算法有独立几何意义，保留各自名称：

- `cutLeft(convex,line)`：保留直线左侧及边界。
- `tangents(point,circle)`、`commonTangents(circle,circle)`：有限条切线由 `Tangents::lines` 返回，无限多条用 `infinite` 表示。`Convex::tangents(point)` 返回两个接触顶点下标，要求外点及非空凸包。
- `relation(circle,circle)`：分离、外切、相交、内切、包含、重合。
- `intersectionArea(circle,circle/polygon/convex)`：明确计算**圆盘**交叠面积，支持对称参数顺序。这是面积计算，不改变 Circle 的圆周求交和最近点语义。
- `farthestPair、diameter、minWidth、minBoundingRect`：旋转卡壳。最远点对返回可选下标对；最小面积矩形返回可选 `BoundingRect`，含四角、面积、该矩形周长。空输入无点对或矩形，直径要求非空，点/线段宽度为零。
- `minkowskiSum、minkowskiDiff`：支持空、点、线段等退化凸包。
- `halfPlaneIntersection(lines,bound)`：必须显式提供有限凸域，不添加虚构的无穷大矩形。

最近点对、最小覆盖圆等具体问题求解器不在模板中。

## 复杂度

| 操作 | 复杂度 |
| --- | --- |
| `convexHull` / `Convex::fromBoundary` | O(n log n) / O(n) |
| `Convex::loc`、外点切线、`inter(convex,line)` 两种模式、`near(convex,line)` | O(log n) |
| `inter(convex,seg)` 两种模式 | O(log n) |
| `near(point,convex)`、`near(convex,seg)` | O(n) |
| 直线/线段/圆之间的 `inter`、`near` | O(1) |
| 圆与多边形/凸包的判定、最近点、交叠面积 | O(n) |
| 一般多边形与直线/线段的求交坐标 | O(n log n)，判定和最近点 O(n) |
| 一般多边形之间，或与凸包的判定、最近点 | O(nm) |
| 旋转卡壳 | O(n) |
| Minkowski、凸包/凸包判定和最近点 | O(n+m) |
| 凸包/凸包交集 | 常规 O(n+m)，退化回退最坏 O(m(m+n)) |
| 半平面交 | 常规 O(n log n+b)，退化回退最坏 O(n(n+b)) |

凸包最近点保留 Minkowski 差的原顶点配对，投影或重心坐标直接恢复两个原对象上的点。没有为了求最近点构造完整凸包交集；圆与凸包操作不复制顶点数组。

## 标量与精度

整数输入点坐标、圆心坐标的绝对值及半径不超过 10^9 时，基础平方距离、点积和叉积的最终值可放进 i64。Point 的整数点积/叉积先用 __int128_t 计算乘积；orient 直接用加宽坐标差判断符号；圆/直线判定的四次乘积及整数面积累加也在内部加宽。对外仍只返回 T，不增加模板参数。调用者保证每项公开数值结果可放进 T；Minkowski 等构造扩大坐标后，后续操作要重新核对结果范围。产生投影、交点、最近点、重心等新坐标的操作需要浮点 T；整数实例给出 static_assert，不截断坐标或隐式更换坐标类型。点与对象求交仅返回原输入点，可以使用整数。

运行时浮点叉积用两次 std::fma 补偿乘积相消误差；C++17 常量求值保留原算术表达式。加宽整数和常量求值检测使用 GCC/Clang 支持的扩展。半平面交若原始点坐标为范围内的整数值、方向分量不超过 2×10^9，则交点的队列侧判定在除法和坐标构造前使用精确整数运算；它仍返回浮点坐标。

`double` 使用原生比较；需要容差时选择包装浮点。几何层不额外添加 EPS，也不把包装浮点转换成 double。包装浮点提供比较容差，不能恢复已经损失的信息。接近退化时的舍入仍可能影响浮点结果；上述补偿和整数值输入路径不是任意实数输入的完整精确几何。自测误差容限仅用于独立数值参考与构造点的归属检查，不写入模板实现。

## 验证与参考

[Modules.md](Modules.md) 记录四文件拆分、容器访问和专项验证结果；[ModuleValidation.json](ModuleValidation.json) 保存本轮源码指纹、命令及结果。[BenchmarkModules.csv](BenchmarkModules.csv)、[BenchmarkModuleSamples.json](BenchmarkModuleSamples.json) 是本轮拆分前后的性能对照。

[Validation.md](Validation.md) 汇总当前与历史验证；[ConvexOptimization.md](ConvexOptimization.md)、[Validation.json](Validation.json) 和其余旧基准文件保留此前凸包/数值优化的历史记录，不能当作本轮源码的指纹或测量。未运行全库测试。

算法参考原有 Original/兼容快照与朋友的静态凸包代码；保留 KACTL 改写部分的 [LICENSE](LICENSE)。

- [KACTL SegmentIntersection](https://github.com/kth-competitive-programming/kactl/blob/main/content/geometry/SegmentIntersection.h)：空、点、重叠线段的求交表达。
- [KACTL LineHullIntersection](https://github.com/kth-competitive-programming/kactl/blob/main/content/geometry/LineHullIntersection.h)：对数时间凸包查询，改写遵守 Boost license。
- [KACTL CirclePolygonIntersection](https://github.com/kth-competitive-programming/kactl/blob/main/content/geometry/CirclePolygonIntersection.h)：圆盘面积边贡献，CC0。
- [Victor Lecomte 的竞赛几何手册](https://vlecomte.github.io/cp-geo.pdf)：位置、投影、求交与退化情况。
- [CGAL 求交](https://doc.cgal.org/latest/Kernel_23/group__intersection__linear__grp.html)、[圆周语义](https://doc.cgal.org/5.4.3/Kernel_23/group__do__intersect__circular__grp.html)：明确对象集合与结果含义。
- [Boost.Geometry closest_points](https://www.boost.org/doc/libs/latest/libs/geometry/doc/html/geometry/reference/algorithms/closest_points/closest_points_3.html)：两个对象上各自的最近点。
- [cp-algorithms Minkowski](https://cp-algorithms.com/geometry/minkowski.html)、[半平面交](https://cp-algorithms.com/geometry/halfplane-intersection.html)：边合并与有序半平面队列。


## 读入点集并输出凸包

```cpp
#include <iostream>
#include "template/Geo2/PolygonConvex.hpp"

using i64 = long long;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<Point<i64>> points(n);
    for (auto &p : points) std::cin >> p;
    auto hull = convexHull(std::move(points));
    std::cout << hull.size() << '\n';
    for (const auto &p : hull)
        std::cout << p.x << ' ' << p.y << '\n';
}
```

输出点数及从字典序最小点开始的逆时针顶点序列。无重复首尾点，去除重复点与冗余共线点；全共线时保留两端点，全重复时只保留一个点。
