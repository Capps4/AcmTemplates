# 竞赛模板与验证工具

`Final.hpp` 是各模块的实现来源，`Original.hpp` 保留原实现用于审查和性能对照。
`Snippets/` 保存本次从 VS Code 导入的 55 个当前 snippet 快照，Manifest 记录其相对路径和 SHA256。`Final.hpp` 保留仓库已有修复，并合入 snippet 的更新内容；原始快照用于审查和对照。验证工具使用头文件，不自动安装或导出 snippet。
算法代码遵循原实现的接口、布局和命名，测试工具与模板实现分开存放。

Git 仓库仅位于本目录，远端为 [Capps4/AcmTemplates](https://github.com/Capps4/AcmTemplates)，参见 [Git 管理](Git.md)。

## 常用命令

以下命令在外层 `acm_compete/` 目录执行；若已进入 Git 仓库 `template/`，去掉命令中的 `template/` 前缀。需要 Python 3.9 或以上。默认编译器为本机 GCC 15 和 Clang，支持 `CXX` / `SAN_CXX` 覆盖。

```bash
# 指定模块：严格编译、优化版正确性、ASan / UBSan
python3 template/Run.py check FenwickTree ExGcd

# 扫描当前源码和依赖，复用有效结果，验证变动影响到的项目
python3 template/Run.py check --affected

# 全部当前范围；加 --force 可忽略缓存重新执行
python3 template/Run.py check --all

# 分开运行；compile 仅编译，不代表正确性测试通过
python3 template/Run.py compile MaxFlow
python3 template/Run.py test MaxFlow
python3 template/Run.py sanitize MaxFlow

# 当前统一验证范围内的头文件共存、跨模块行为、原注释功能切换
python3 template/Run.py integration

# 性能测试始终重新运行，不使用旧计时结果作当前结果
python3 template/Run.py bench FenwickTree ExGcd

# 查看每个项目的当前状态，以及最新运行报告的位置
python3 template/Run.py status
python3 template/Run.py report

# 验证当前范围完成后，交付带模块目录的头文件副本
python3 template/Run.py package

# 仅清理新工具的构建目录，报告和基线保留
python3 template/Run.py clean
```

`--jobs 3` 控制编译及测试并发；`--timeout 180` 是每条编译或运行命令的秒数上限。
单项失败仍收集其他项目的结果，整个命令最终返回非零。失败命令、输入和输出在报告中保留。
`--seed 20261001` 通过 `TEST_SEED` 控制 TestSupport 的随机数；测试里自行固定种子的用例保持原行为。

严格编译参数固定为：

```text
-std=gnu++17 -Wall -Wextra -Werror -Weffc++
-O0 -g -rdynamic -fno-omit-frame-pointer
```

正确性另用 GCC `-O2 -Wall -Wextra`；sanitizer 使用 Clang `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer`，首次错误即失败。
性能使用 GCC `-std=gnu++17 -O2`，不用 sanitizer，不加 `-DNDEBUG` 隐藏校验。

## 文件与结果

统一验证模块通常保留 `Original.hpp`、`Final.hpp`、`Test.cpp`、`Benchmark.cpp`、`README.md`；专项模块保留各自的示例与验证入口。
`TestExtra*.cpp` 与主测试一起链接；`TestCase*.cpp` 分别编译、运行。
依赖和暂缓范围集中在 `Manifest.json`。Geo2/Geo3、Trie、SparseSegTree、TreeMap 保留专项入口或源码，但尚未纳入统一验证；这些模块不会被 `Run.py check --all` 或 `package` 自动包含。

- `Build/`：可清理的程序、对象、编译器依赖文件和生成的集成测试源码。
- `Reports/<运行编号>/`：Results.json、Summary.md、日志和性能 CSV。
- `Reports/State.json`：各项目最新验证记录；检查源码指纹后才能复用。
- `Reports/Latest.json`：最近一次运行的报告位置，局部运行不表示全库通过。
- `Baselines/<名称>/`：不可覆盖的源码、依赖及结果快照。
- `ReadyHeaders/`：通过当前验证的头文件交付目录，保留相对 include 所需的模块结构。

缓存分别记录源码、测试及其传递依赖、编译器路径/版本/目标、参数、相关 include / SDK 环境变量、测试输入和随机种子。
编译器实际生成的依赖文件也参与有效性检查。修改测试不会强制跑 benchmark。
`--affected` 扫描全部当前项目，跳过有效记录，并检查集成测试；新增用例也会被发现。
旧的模块 Results.json / Performance.md 和 VerificationHistory 是历史资料，新入口不会把它们当成当前验证证据。
状态中的 `stale` 表示已过期，`compiled` 仅表示编译成功，`passed` 表示该项目已编译且运行成功。

## 正确性用例

沿用现有穷举、朴素对照及边界用例，不引入测试框架。新增测试优先按以下顺序：

1. 固定边界及历史回归：空输入、重复操作、数值边界、缓存重用。
2. 小规模穷举，与独立 oracle 比较。
3. 固定种子的随机对照；失败输出种子及输入，修复后留下固定回归用例。
4. 较大或特殊结构：长链、有序输入、高重复率、反复增删。

Original 不是正确性的唯一依据；只检查模块承诺支持的输入和接口。
`CHECK` 自动打印行号和 TEST_SEED，涉及随机输入的测试应额外打印可复现数据。
原注释功能切换在 `Options.py` 中生成独立源码测试，不改默认实现。
跨模块检查集中在 `Integration/`，不再附着在 FFT 的单模块测试中。
基础设施自己的回归测试：`python3 template/InfrastructureTest.py`。它在临时目录验证缓存失效、依赖、编译失败、sanitizer 失败、超时和 checksum，保留工程模板不变。

## 性能测试与基线

所有受新入口管理的编译、测试和计时共用进程锁。内部可并行验证；benchmark 先完成全部编译，再串行计时。
请同时关闭其他重负载任务；工具不能阻止用户自行启动的其他程序。

输入应在计时前生成，明确区分构建、预处理、查询、修改和遍历。每个 `compare` 回调必须可重复，修改型工作负载应每次重置自己的数据。
冷缓存等固定次数场景可用 `compare(name, before, after, false)` 禁用批量重复，一次校验、一次预热、七次计时。
工具先校验两版 checksum 并预热，短场景使用相同批量次数，默认目标至少 10ms，最多 1024 次；校准中任一版达到 100ms 就停止增加批次，避免快慢差距很大时重复慢版过久。
每轮交替 A/B、B/A，七轮取中位数；报告保存全部样本、批量次数、MAD/中位数波动。
CSV 的毫秒和倍率按一次回调折算，场景名应写明规模及操作数。不能把不同工作负载的倍率直接平均。
较短、波动较大或差距很小的场景标记“尚无明确收益”；此判断只是筛选，不是统计显著性保证。
如果 1024 次仍达不到足够时间，应扩大场景，不依据该倍率接受优化。

```bash
# 在开始优化前，检查并计时待优化模块
python3 template/Run.py check FenwickTree
python3 template/Run.py bench FenwickTree
python3 template/Run.py baseline beforeFenwick

# 修改 Final.hpp 后，验证并与快照里的 Final 同期重新测量
python3 template/Run.py check FenwickTree
python3 template/Run.py bench FenwickTree --baseline beforeFenwick
```

默认报告 Benchmark.cpp 中的对照版本 / Final，计时协议将对照侧记作 Original。大多数模块对照 Original.hpp；StringHash 使用 BenchmarkBefore.hpp，ListHelper 使用 CurrentSource.hpp，具体以场景源码为准。命名快照对照会重新编译快照版本及当前版本，在七轮程序执行中交替先后顺序，测量各自的 Final，另外报告快照 / 当前 Final。
快照针对最近一次 benchmark 中通过的模块，同时保存参与验证的源码和依赖；这些模块的正确性检查也必须有效。不依赖 Git，也不覆盖已有名称。
同名基线不自动替换。场景源码、计时辅助代码、checksum、编译器、参数、种子或平台不同会拒绝比较，应建立新基线。
`--min-ms` 改变批量计时目标；改参数后需要对应基线。不能把不同机器或参数下的历史耗时直接当成优化收益。

新增模块时在 Manifest 注册依赖及可选暂缓原因，增加 Test.cpp、Benchmark.cpp 和 README，即可使用同一入口。
模块 README 写接口、契约、重要切换与场景；当前性能数字引用 Reports，避免重复维护旧数字。

## 2026-10-04 同步与目录整理

本次纳入的 snippets 中，41 个仍与 Original 快照一致，保留 Final 中后续改进；FloatPointNumber、Kmp、StringHash 的更新和 ListHelper 的排序更新已在仓库中。HashMap 保留仓库的 `_hashmap::rngSubSqrt` 名称限定修正，Snippets 中原文件按原样存档。

SegTree 与四层 Geo2 头文件匹配当前 snippet 内容。Geo3 从当前 snippet 更新到 `Geo3/Final.hpp`；Trie、SparseSegTree、TreeMap 的核心实现匹配当前 snippets，专项示例和资料已迁入对应模块。TreeMap 改为依赖 `ListHelper/Final.hpp`，ListHelper 的旧专项资料保存在 `VerificationHistory/ListHelperExperiments/`。

不导入 init_ / init_case，当前模块目录和集成入口生成逻辑已移除。当前 snippets 已删除的 LazySegmentTree、PersistentTree、Point 也移除；TreePre 原本就不存在。SegTree 的旧 Lazy 实现仍作为 Original.hpp 中的性能对照快照。

Baselines、VerificationHistory 和历史测量结果保留当时内容及旧路径，不当作当前源码或本次验证证据。源码变化会使现有验证缓存过期；旧 ReadyHeaders 已移除，需验证后重新 package。本次仅完成同步、迁移及源码差异 Review，未执行编译或测试。
