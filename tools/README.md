# CTL

Capps Template Library 的唯一命令入口。Python 3.9+。

| 命令 | 用途 |
| --- | --- |
| `ctl --help` | 帮助 |
| `ctl update --yuque` | 更新语雀完整文档 |
| `ctl update --local` | 安装本地 VS Code snippets |
| `ctl rule` | 严格规则检查 |
| `ctl test --right` | 正确性测试 |
| `ctl test --perf` | 性能测试 |
| `ctl test --all` | 两种测试都执行 |

更新/规则命令显示阶段；测试每个模板只显示一行结果，失败给出阶段、case、种子与复现命令。终端使用颜色，重定向输出或设置 `NO_COLOR` 时使用普通文本。任意工作目录均可调用。

测试运行期间显示工具回归、编译、运行/测量及报告阶段，并每 5 秒刷新已完成任务数、阶段耗时和当前任务。计数按测试任务计算（包含编译模式与独立入口），最终结果按模板汇总。关闭颜色或非交互输出时同样保留进度，使用普通文本行；测试结束或中断后停止刷新。

## 安装入口

在仓库根目录执行一次：

```bash
mkdir -p ~/.local/bin
ln -s "$PWD/ctl" ~/.local/bin/ctl
```

将 `~/.local/bin` 加入 PATH；未安装时也可在仓库根目录使用 `./ctl`。
本地 snippets 只需要 Python 标准库；语雀文档需要：

```bash
python3 -m pip install -r tools/requirements.txt
```

## 统一配置

配置集中在 [__init__.py](__init__.py)：

- `SNIPPETS_DIR` / `VSCODE_USER_DATA`：默认自动查找；多候选或自定义目录时配置其中一个。
- `MIGRATE_LEGACY_SNIPPETS`：自动备份并迁移可识别的旧模板，默认开启。
- `YUQUE_DOCUMENT`：发布目标；`YUQUE_TOKEN`：Token 文件路径。
- `CXX` / `SAN_CXX`、`FLAGS`：编译器与严格/优化/Sanitizer 参数；支持环境变量指定编译器。
- `TEST_MODULES`：默认全库；可配置模块元组作为局部工作范围。
- `TEST_JOBS`（编译并发默认 6）、`TEST_RUN_JOBS`（正确性运行并发默认 32）、`RIGHT_PROFILES`、`TEST_CASE`、`RIGHT_SEED` / `PERF_SEED`、`TEST_TIMEOUT` / `COMPILE_TIMEOUT`：测试参数。
- `PERF_DEFAULT_SIZES` / `PERF_REPEATS`（默认 3）：默认性能规模和奇数采样次数；模块可在对应性能目录的 `Settings.json` 配置两档规模与固定 `iterations` 批次。
- 文档、缓存、报告、语雀 CLI 版本与请求超时等路径和设置。

配置文件只保存 Token 的路径。Token 值单独保存在忽略的 `.yuque/token`，权限 600，或使用 `YUQUE_TOKEN` 环境变量。

## 更新本地 snippets

```bash
ctl update --local
```

自动查找 macOS、Windows、Linux 的默认 VS Code / Insiders 用户配置、Profiles、便携版与 VS Code Server；WSL 中也会尝试寻找 Windows 配置。snippets 目录尚未创建时会按需创建。交互终端遇到多个候选时输入编号选择；找不到时可输入自定义目录。非交互环境会列出提示，需在 init 中指定 `SNIPPETS_DIR` 或 `VSCODE_USER_DATA`。

只维护 `acm-templates.code-snippets`，个人 snippets 和 `init_`、`init_case` 保留。重复前缀或无法解析的文件会阻止安装。旧模板迁移及受管理文件更新前备份到 `.cache/snippet-backups/`。

在 C++ 编辑器输入 `_T_模块名`。兼容前缀由 Manifest 的 `prefixes` 配置；旧文件名由 `aliases` 配置。
Snippets 只包含模板正文，前置模板按清单和模块 README 插入。

## 供他人复制运行的安装器

根目录 `Library.md` 的 `<!-- @tool SnippetInstaller -->` 在文档生成时展开成完整 Python 脚本。用户从语雀复制全部代码，保存为 `install-snippets.py`，执行 `python3 install-snippets.py`（Windows：`py -3 install-snippets.py`）。Python 3.9+，只使用标准库，运行时无需联网或本仓库。

脚本内置从当前 Manifest 和源码生成的模板数据及旧文件识别信息，安装代码直接取自共享的 `SnippetInstaller.py`。`ctl update --local` 通过 Fetcher 调用同一模块，目录选择、冲突检查、备份、迁移和原子写入只维护一份实现。独立脚本不会携带维护者的本地目录或凭据配置。

可用 `--dir` 指定 snippets 目录，或用 `--user-data-dir` 指定 VS Code 用户数据目录。独立脚本的备份保存在系统用户缓存下的 `acm-templates/snippet-backups/`；本地 CTL 继续备份到仓库 `.cache/snippet-backups/`。个人 snippets 与初始化模板保留。

执行 `ctl update --yuque` 时，按已有发布流程自动生成并同步安装器代码卡片。模板或共享安装逻辑更新后，下一次文档生成自动包含变更；用户需重新复制最新脚本以更新。展开后的文档位于 `.cache/generated/Library.md`，根目录 `Library.md` 保留引用标记。

## 更新语雀

正文、标题和排列顺序维护在根目录 `Library.md`；模板代码维护在 `src/`。用独占一行的标记引用源码：

```markdown
### 模块名

这里写模块说明。

<!-- @code 模块名 -->
```

分层模板使用 `模块名/层名`；模块路径、依赖和有序的 `layers` 在 `Manifest.json`。各层按清单顺序包含前一层，第一层使用模块的 `include` 或公共头入口。每个模板必须引用一次。普通代码围栏用于补充说明，不进入 snippets；`<!-- @tool CTL -->` 展示当前命令入口。

`<!-- @tool SnippetInstaller -->` 展示共享安装逻辑与当前模板数据组装的独立安装器。生成来源记录包含安装模块、生成工具、清单及模板源码的指纹，发布前检查来源是否变化。

支持 Markdown 常用结构、表格与公式；原始 HTML 会报错。`Library.json` 只保存标题锚点、卡片 ID 和显示设置。

```bash
ctl update --yuque
```

需要 Node.js 20+。首次使用会安装固定版本的官方 CLI；缺少 Token 时，在交互终端隐藏输入，非交互环境给出配置提示。Token 在 https://www.yuque.com/settings/tokens 创建，需要文档编辑权限。

流程包括生成正文、认证、检查远端差异和现版测试报告、备份发布、回读验证。生成结果与差异在 `.cache/generated/`；内容不变时跳过写入。更新不会自动运行测试，发布要求全库正确性与性能的有效通过记录。

如果远端被手动修改：先把需要的修改合入 `Library.md`，检查差异文件，再将 init 中的 `YUQUE_EXPECTED_REVISION` 设为输出的精确快照值。成功更新后清空该项。它只确认这一版快照，不自动合并。

备份在 `.yuque/backups/`，发布状态在 `.yuque/publish/`。超时或回读不一致时，写入可能已发生，先检查远端、`after.json` 和备份，避免直接重试。API 读取和写入之间仍存在很短的并发窗口。

## 规则与测试

`ctl rule` 严格扫描模板，违规和未处理的候选都返回非零；保留理由在 `RuleExceptions.json`。它不能替代正确性或性能测试。

正确性使用严格调试、优化、ASan/UBSan 三种配置；主入口合并模块内 oracle、边界及回调，仅配置隔离检查保留独立入口。全库范围还运行工具回归。有效编译产物可复用，但每次重新执行测试；证据必须匹配源码、测试数据、依赖、编译器、参数、种子与完整 case 清单。性能先并行编译、并行启动等待中的进程，准备完成后逐个唤醒顺序测量，两档规模、两种形状，每组预热一次、采样三次；同一入口的四组负载合并在一个进程中顺序执行，输入分别重建，峰值 RSS 记录为进程级数据。

`ctl test --all` 会执行两类测试，即使正确性失败也继续收集性能结果；最终返回失败并保存同一份 `.cache/reports/Verification.md`。只有该测试报告保留，当前二进制及编译元数据保存在 `.cache/compiled/`；对象、依赖文件和临时输出删除；报告内压缩证据继续支撑发布门禁。文档生成、规则报告和其他工作流备份不删除。部分 profile/case 通过不代表完整发布验证通过。

局部复现示例：

```bash
ctl test --right --module SegTree --case orderedMerge --profile optimized --seed 42
ctl test --perf --module CostFlow --seed 20261005
```

`--module` 可重复；`--case` / `--profile` 用于正确性。默认编译并发为 6，正确性运行并发为 32；性能测量阶段无编译并发。缓存命中后的全库目标是正确性命令 10 秒、性能命令 15 秒，首次编译单独计时；每模块正确性 20 ms 与性能 100 ms 是工作量上限，不包含编译和启动，也不把严格版/Sanitizer 运行时间混入优化版结果。

测试执行器目前需要 macOS 或 Linux；Windows 可使用 WSL。详细覆盖与边界见 [tests/README.md](../tests/README.md)。

内部模块分别负责清单、文档、snippets、语雀客户端和通用测试执行，均由 `ctl` 调用。工具不按算法名称分支、不改写模板源码；具体测试需求在对应的 tests 模块目录维护。
