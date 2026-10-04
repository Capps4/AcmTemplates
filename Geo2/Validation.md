# Geo2 专项验证

2026-10-04：实现拆分为 PointVec、SegLine、PolygonConvex、Circle 四层，补充容器式访问并导出全局接口。

- [本轮文件结构、接口与验证报告](Modules.md)。
- [本轮源码指纹、构建参数及测试输出](ModuleValidation.json)。
- [本轮拆分前后性能对照](BenchmarkModules.csv)、[原始测量及代表性函数汇编对照](BenchmarkModuleSamples.json)。

Geo2、Point 及兼容入口进行专项回归，覆盖 Clang/GCC、Clang ASan/UBSan；未启动全库测试。基于用户最新浮点数实现重新执行后，144 项检查全部通过；对应源码指纹核对一致。

## 历史记录

- 2026-10-02：[凸包优化报告](ConvexOptimization.md)、[当时的源码指纹和检查](Validation.json)、[优化前后基准](BenchmarkConvex.csv)、[朋友实现对照](BenchmarkFriend.csv)、[原始测量](BenchmarkConvexSamples.json)。
- 数值优化：[报告](NumericOptimization.md)、[当时的源码指纹](NumericValidation.json)、[优化前后基准](BenchmarkBeforeAfter.csv)。
- 此前的公开操作及 KACTL 对照：[Benchmark.csv](Benchmark.csv)、[BenchmarkComparison.csv](BenchmarkComparison.csv)。

这些历史指纹和基准对应此前文件结构与源码，不能当作当前拆分版本的验证结果。
