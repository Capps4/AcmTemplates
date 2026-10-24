# 测试

Python 3.9+，执行器支持 macOS/Linux，Windows 可使用 WSL。优先使用 GCC 15，ASan/UBSan 使用 Clang；编译器可由 `CXX` / `SAN_CXX` 指定。

```bash
ctl test --right
ctl test --perf
ctl test --all
ctl test --right --module SegTree --case orderedMerge --profile optimized --seed 42
ctl test --right --module Geo2 --case dense-conditioning --profile optimized
```

`Correctness/` 和 `Performance/` 按 src 的集合、分类及主题层级镜像。两套算法遵守相同的严格编译、oracle、边界和性能要求；没有因 extra 身份而降低要求。

`ctl test --all` 默认覆盖全仓；`--collection core|extra` 选择集合及传递依赖，完整集合范围同时执行共享工具回归。发布按目标集合及依赖核对现版证据，单一报告继续保存全仓记录。

`Correctness/` 维护模块用例。覆盖清单在 [Cases.json](Correctness/Cases.json)，每项记录风险、oracle、算法头和源码位置；没有最低 case 数量。小规模穷举、固定种子随机与手工回归分别验证契约、状态转换和已知故障。相同 API/风险的重复规模不另计覆盖。

`Test.cpp` 包含核心 oracle、边界与回调用例。`TestExtra*.cpp` 与主入口一起链接，仅用于实际需要跨翻译单元共享状态的 RandomNumber、FloatPointNumber、StringHash。`TestCase*.cpp` 独立编译，只保留需要独立宏配置、类型/数值边界或头文件包含环境的检查。Demo 保留为可阅读示例，其必要行为归入主入口；Trie 直接复用 demo 查询函数。SegTree 默认 Info/Tag 宏重命名检查保留独立入口。

严格调试、优化、ASan/UBSan 使用同一组核心行为。严格参数保留：

```text
-std=gnu++17 -Wall -Wextra -Werror -Weffc++ -O0 -g -D_GLIBCXX_DEBUG
```

四个自动机保留朴素子串/回文/匹配 oracle，以及空/重复串、回调、clone、分段追加、optional、全字节和外部汇总。FFT/NTT 保留 127/128/129 切换点；FastIO 保留 1 MiB 缓冲跨界；TreeMap 保留候选排序阈值、资源释放和复制/移动。Geo2 保留独立层头编译及固定半平面回归文件。65536 顶点的数值条件用例作为显式 `dense-conditioning` stress case，不进入常规预算。

目标为编译缓存命中后的全库正确性命令 10 秒、性能命令 15 秒；首次全量编译单独计时。模块优化版正确性工作量上限 20 ms，性能预热/采样工作量上限 100 ms，不设置需要填充数据的下限。编译、进程启动、严格版与 Sanitizer 开销分开记录；不为耗时下限填充无覆盖价值的数据。规模与批次按本机全量自测校准；以报告中的实测工作量为准。规模、形状、种子和 oracle 固定，不在运行时缩小数据来掩盖回归。

`Performance/` 的 `Benchmark*.cpp` 采用两档规模、两种形状，每组预热一次、采样三次。全部性能进程先并行启动并阻塞等待，启动线程全部结束后再逐个唤醒测量，不与其他测量或进程启动重叠。同一入口的两档规模、两种形状在一个进程中顺序调用，各组重新构造输入；峰值 RSS 是四组共用的进程峰值，不能当作单组峰值。输入生成在计时前，构造算法对象在计时内；记录预热、各样本、中位数、校验值和峰值 RSS。自动机先用小输入 oracle 验证计时函数；排序/拓扑/重心遍历使用顺序敏感校验，MaxFlow 使用深链与宽增广网络及已知最小割。其余结果正确性由对应 Correctness 的独立 oracle 验证。工作负载与必要不变量检查均以源码为准，本机结果不证明 OJ 前 5%。

每个性能目录用 `Settings.json` 指定两档递增正整数和固定批次，例如 `{"sizes": [1000, 10000], "iterations": 4}`。每个预热/样本完整执行 4 次内核并汇总校验值；批次增加测量信号，不增加输入内存。含多个 benchmark 的目录可用 `"iterations": {"BenchmarkFirst": 4, "BenchmarkSecond": 2}` 分别配置。未配置时使用 `tools/__init__.py` 的 `PERF_DEFAULT_SIZES`。经确认无需测量的模块可保留 `Benchmark.cpp` 占位，并在 `Settings.json` 设置非空 `"skip"` 原因；执行器记录带源码/配置指纹的跳过证据，报告标明“无需性能测试”，不编译占位或生成耗时样本。其他模块仍要求完整性能测量；删除原因或修改源码后旧证据失效。默认最多 6 个编译任务、32 个正确性运行任务；性能全部编译完成后，串行测量。递归图测试保留专用线程栈；通过测试不代表默认 OJ 栈容量足够。

终端每个算法头只输出一行结果；失败另附阶段、case、种子、退出码/信号/超时及可用的输入、期望/实际值，并给出复现命令。`--case` 支持完整 ID 或模块内后缀，具名 case 的随机序列由种子和 ID 决定。未匹配 case 返回失败。`--profile` / `--case` / `--seed` 的局部执行不能替代全库发布验证。

模块文件清单按原始字节纳入证据；include 依赖只从当前编译入口遍历，所以一个独立坏 case 不会污染同模块的正常入口。编译器生成的依赖列表还记录当前实际 include。所有证据随源码、数据、工具、编译参数和种子失效；旧 PASS 不能证明现版通过。全库正确性另运行 `Tools/` 回归，执行器集成测试仅编译一个极小模块。

测试报告保存在 `.cache/reports/Verification.md`，编译产物保存在 `.cache/compiled/`，每个测试入口只保留当前版本。编译缓存核对入口及翻译单元集合、include、编译器、编译参数、环境、构建工具和二进制指纹；种子、case 选择、输入数据和运行规模只使验证证据失效，未改变编译输入时复用二进制。性能的采样次数和固定批次目前是编译宏，改变它们仍须重新编译。文件指纹按 mtime、ctime、大小、inode 和设备号更新，同一阶段去重，测试结束重新校验实际依赖。每次重新执行正确性 case 和性能采样，不复用 PASS 或测量结果。

报告显示最近调用的范围、实际耗时和缓存命中，另按 strict/O2/Sanitizer/性能汇总当前有效记录的内核、计时外和编译工作量，并列出最慢用例。进程累计量可能相互重叠，不能代替命令总耗时；性能的启动与计时外准备分别记录。样本中位数不足 0.01 ms 或极差超过中位数的 50% 时提示测量信号不足或波动，不影响通过状态，也不代表性能退化。完整样本、失败详情和指纹保存在压缩的机器证据中，供发布门禁核对。

除当前编译缓存外，对象、dep 文件和临时输出在正常结束、失败及可处理的中断后删除；SIGKILL/断电可能遗留的测试临时目录在下次持锁运行时清理。旧测试 build/benchmarks、历史日志和旧 JSON 报告随新报告保存后清理；文档生成、规则报告、snippets 和语雀备份保留。全库/局部测试都使用同一份报告，局部执行替换所选模块记录，不能保留旧的其他 profile 来凑出完整通过。

测试命令受 `.cache/.test.lock` 串行保护，锁文件保留以维持等待者使用的 inode。`ctl test --all` 在正确性失败后仍收集性能结果。运行中的记录先标为未完成，避免中断后继续沿用旧通过记录。详细命令与发布边界见 [CTL 说明](../tools/README.md)。

## extra 测评

extra 的可编译模块以 Manifest 为准；DigitDP、Block、DsuOnTree 是题目相关框架说明，不计入编译模块。常规验证运行 `ctl test --all --collection extra`，覆盖 extra、传递依赖和共享工具回归。无需为整理测评扩大到其他 core 模块。现版验收状态以报告中的源码、编译参数及硬件指纹为准。

迁移回归额外检查全仓引号 include 的可解析性，以及两份文档的模块归属、完整覆盖和重复引用拒绝。算法用例补强了 LiChaoTree 的 min/max 独立包络与真实交叉线、WeightedDSU 的反向合并和 BFS 势差、两种 Euler 的精确类型与边消费、Delaunay 的内点及完整空圆三角形集合、TreeHash 的固定碰撞回归和 200 种小规模有根树结构对照。Delaunay 性能场景仍为凸位置抛物线的顺序/乱序插入，内点覆盖目前属于正确性测试。

本轮新增 FloorSum、BitwiseTransform、LinearRecurrence、RollbackDSU、SlidingWindowAggregation、SparseMatBIT、BipartiteMatching、LowerBoundFlow、Rerooting 九个 extra 模块。矩形消元用 GF(3) 解空间穷举；上下界流用边流穷举；匹配与最小覆盖各自穷举；换根 DP 用逐根 BFS 与保持邻接顺序的字符串 oracle。回滚、非交换队列和稀疏二维树状数组分别对照重建连通分量、逐函数应用与原始坐标求和。额外覆盖 UINT64_MAX、128 位结果溢出、LLONG_MIN 逆变换、非默认构造值及深链。

性能对照包含 ClosestPair 的逐层排序旧实现、Mo 的固定平方根块长、二分图匹配的 core Dinic 归约、线性递推的伴随矩阵幂和换根 DP 的专用距离和实现。成对基准使用相同输入，按相同结果顺序核对 checksum；不同场景分别报告。SparseMatBIT 的常规结构用例固定 512 点，查询首尾、二次幂邻域及分层位置；优化性能场景仍覆盖 10000 点。SlopeTrick 保留每步网格 DP 对照，减少重复强制最优点探测，median 结构用例为 512 项。WeightedDSU 和动态连通性复用同一状态的独立 BFS 结果，保留所有点对断言。

后续新增的 14 项包括 ModSqrt、DiscreteLog、LagrangeInterpolation、SlopeTrick、SegmentTreeBeats、ImplicitTreap、LyndonFactorization、RunEnumeration、LinkCutTree、BellmanFord、OfflineDynamicConnectivity、GeneralMatching、DirectedMST、MinimumEnclosingCircle。它们分别对照全根/幂状态枚举、Horner 求值、网格 DP、普通数组、字符串拼接、全部分解/周期枚举、BFS 路径、Floyd、逐时刻重建、子集匹配、入边组合穷举和支撑圆枚举。圆的 oracle 先归一化坐标，避免绝对容差掩盖小尺度半径错误。

较重的离散对数全状态、Runs 完整二进制串枚举和长周期串、六点全图匹配、20K 嵌套缩环保留为显式 stress case，可用 `ctl test --right --module <模块> --case <完整 ID>` 运行。常规用例保留每类风险的小规模 oracle、固定回归、随机和结构检查；性能维持两档规模、两种形状及预热/三次采样。RunEnumeration 另外对照 Nyaan 的分治实现，并在小输入上逐项比较完整结果。最终是否通过及实测工作量以当前 Verification 报告为准。

性能的暖机 checksum 只验证后续执行的一致性。结果正确性由独立预期或合法性检查补足：最小表示核对已知最小旋转，FFT 核对直接卷积系数及冲激结果，堆核对排序后的剩余序列，BIT 对照普通数组/离线差分，图结果核对恢复边和连通结构。顺序有意义时使用顺序敏感校验。输入须触发目标操作：动态连通性包含真实断开/重连，匹配包含多层增广，矩形操作包含内部边界，最小覆盖圆包含三点支撑。参考答案准备放在计时外，报告仍记录其进程成本。
