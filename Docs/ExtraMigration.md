# extra 迁移记录

来源：https://www.yuque.com/capps/template/library_extra

原文版本：aeae38574158e98a5184577b842ffd10ccf9747826bf97a1298a3605b65d25a3

新链接版本：dcf3afa602e110a4dcf3f7b73bf494fa92aba2d62e5c531f455788484fab9276（文档 ID、Markdown 正文和算法代码均未改变）。

本地实现通过独立测试验收，现版状态以 `.cache/reports/Verification.md` 的有效证据为准；原文代码不会自动成为 PASS。迁移回归已修复 include 和文档引用，并补强六个新模块的测试；TreeHash 经确认替换了存在固定碰撞的原混合公式，保留接口。删除项仅记录名称，不导入维护源码。

| 原卡片 | 维护位置 |
| --- | --- |
| ExCrt | [ExCrt](../src/extra/Math/ExCrt/README.md) |
| next k-bit | [EnumComb](../src/extra/Math/EnumComb/README.md) |
| cntPrimeLessEqual | [RangePrimeCount](../src/extra/Math/RangePrimeCount/README.md) |
| 3DpartialOrder | [PartialOrder3D](../src/extra/Sequence/PartialOrder3D/README.md) |
| MonotonyStack | [MonotonyStack](../src/extra/DataStructures/MonotonyStack/README.md) |
| SparseTable | [BitRMQ](../src/extra/DataStructures/BitRMQ/README.md) |
| WeightedDisjointSetUnion | [WeightedDisjointSetUnion](../src/extra/DataStructures/WeightedDisjointSetUnion/README.md) |
| LiChaoTree | [LiChaoTree](../src/extra/DataStructures/LiChaoTree/README.md) |
| RangeBIT | [RangeBIT](../src/extra/DataStructures/RangeBIT/README.md) |
| MatBIT | [MatBIT](../src/extra/DataStructures/MatBIT/README.md) |
| RangeMatBIT | [RangeMatBIT](../src/extra/DataStructures/RangeMatBIT/README.md) |
| TreeDiameter | [TreeDiameter](../src/extra/Tree/TreeDiameter/README.md) |
| DecareTree | [DecareTree](../src/extra/Tree/DecareTree/README.md) |
| TreeHash | [TreeHash](../src/extra/Tree/TreeHash/README.md) |
| BiGraphColor | [BiGraphColor](../src/extra/Graph/BiGraphColor/README.md) |
| DirectedEulerRoad | [DirectedEulerRoad](../src/extra/Graph/DirectedEulerRoad/README.md) |
| UndirectedEulerRoad | [UndirectedEulerRoad](../src/extra/Graph/UndirectedEulerRoad/README.md) |
| twoPointerWithPolarSorting | [PolarTwoPointer](../src/extra/Geometry/PolarTwoPointer/README.md) |
| ClosestPair | [ClosestPair](../src/extra/Geometry/ClosestPair/README.md) |
| DelaunayTriangulation | [DelaunayTriangulation](../src/extra/Geometry/DelaunayTriangulation/README.md) |
| DigitDP | ExtraLibrary.md 的 Examples and Notes |
| Block | ExtraLibrary.md 的 Examples and Notes |
| DsuOnTree | ExtraLibrary.md 的 Examples and Notes |

直接删除：StringHash With Node、HashMap、PrefixSum、pbdsTree、RangeQuerySegmentTree、SegmentTreeMerge、TreeHandler、StaticTreePre、RingTreeForestPre、3D Point、3D ConvexHull、stacktrace。

原 core 一次迁入 11 个模块；core 保留 42 个模块。extra 新接入 20 个正式算法、保留 3 项框架说明，合计维护 31 个算法模块。
