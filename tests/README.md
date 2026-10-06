# 测试

正确性与性能分开运行。需要 Python 3.9+；安装 ctl 后可在任意工作目录执行。优先使用 GCC 15，Sanitizer 使用 Clang，可用 CXX/SAN_CXX 指定编译器。

```bash
ctl test --right
ctl test --perf
ctl test --all
```

`Correctness/` 按 src 层级保存暴力对照、边界、随机、规模、示例及集成用例；统一入口覆盖 Manifest 的 52 个模块，包括 Geo2 分层、跨翻译单元检查与精确 Fraction 半平面 oracle。每个算法头至少十个具名 case，清单在 [Cases.json](Correctness/Cases.json)；数量不代替 API 和输入域覆盖。

正确性分别在严格调试、优化、ASan/UBSan 三种配置下运行。严格参数：

```text
-std=gnu++17 -Wall -Wextra -Werror -Weffc++ -O0 -g -D_GLIBCXX_DEBUG
```

四个自动机的测试额外覆盖回调时机、重复/空串、节点复用、零贡献 clone、分段追加、optional 查询、全字节与外部汇总，使用朴素子串/回文/匹配 oracle。示例均有独立 main；Trie 的主测试复用三个 demo 的查询函数。

`Performance/` 保存独立 `Benchmark*.cpp`；两档规模、两种形状，预热一次、重复七次，进程顺序计时。输入生成不计时，记录全部样本、中位数、稳定校验值和进程峰值 RSS。四个自动机的计时函数在测量前另用小输入 oracle 校验；其他模块的结果正确性由 Correctness 中独立用例覆盖。具体工作负载见源码及 JSON，本机结果不证明 OJ 前 5%。

连通性/TwoSat 的深链正确性用例使用 256 MiB 线程栈，VertexBC 的性能用例亦如此；递归实现通过这些测试不代表默认 OJ 栈容量足够。

局部工作范围、并发、超时、种子和正确性缓存重跑设置在 `tools/__init__.py`，不再提供独立脚本命令。`TEST_MODULES` 默认为全库。

`Support/` 保存共享 oracle 和工具头；`Tools/` 保存命令、文档生成、snippets、语雀写入边界和执行器回归。全库正确性模式会运行工具回归，其中执行器测试使用隔离的微型仓库。

所有日志、构建产物和报告写入 `.cache/`。Verification 核对任务清单、具名 PASS 输出及源码/依赖/编译器/参数指纹，生成 `.cache/reports/Verification.md`。失效、缺失或未运行的记录均不算通过；目录里有用例不代表现版已测试。

性能完整覆盖需要执行全库范围；不再提供手动合并报告命令。详细流程见 [CTL 说明](../tools/README.md)。
