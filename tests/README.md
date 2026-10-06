# 测试

Python 3.9+，执行器支持 macOS/Linux，Windows 可使用 WSL。优先使用 GCC 15，ASan/UBSan 使用 Clang；编译器可由 `CXX` / `SAN_CXX` 指定。

```bash
ctl test --right
ctl test --perf
ctl test --all
ctl test --right --module SegTree --case orderedMerge --profile optimized --seed 42
ctl test --right --module Geo2 --case dense-conditioning --profile optimized
```

`Correctness/` 按 src 层级维护模块用例。覆盖清单在 [Cases.json](Correctness/Cases.json)，每项记录风险、oracle、算法头和源码位置；没有最低 case 数量。小规模穷举、固定种子随机与手工回归分别验证契约、状态转换和已知故障。相同 API/风险的重复规模不另计覆盖。

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

测试报告保存在 `.cache/reports/Verification.md`，编译产物保存在 `.cache/compiled/`，每个测试入口只保留当前版本。缓存核对源码、依赖、编译器、参数、环境及二进制指纹；文件指纹按 mtime、ctime、大小、inode 和设备号更新，同一阶段去重，测试结束重新校验实际依赖；源码变化或损坏时重新编译。每次重新执行正确性 case 和性能采样，不复用 PASS 或测量结果。报告包含简表、失败详情和压缩的机器证据供发布门禁核对。除当前编译缓存外，对象、dep 文件和临时输出在正常结束、失败及可处理的中断后删除；SIGKILL/断电可能遗留的测试临时目录在下次持锁运行时清理。旧测试 build/benchmarks、历史日志和旧 JSON 报告随新报告保存后清理；文档生成、规则报告、snippets 和语雀备份保留。全库/局部测试都使用同一份报告，局部执行替换所选模块记录，不能保留旧的其他 profile 来凑出完整通过。

测试命令受 `.cache/.test.lock` 串行保护，锁文件保留以维持等待者使用的 inode。`ctl test --all` 在正确性失败后仍收集性能结果。运行中的记录先标为未完成，避免中断后继续沿用旧通过记录。详细命令与发布边界见 [CTL 说明](../tools/README.md)。
