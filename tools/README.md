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

过程显示阶段、进度、耗时和结果；失败给出原因和日志位置。终端使用颜色，重定向输出或设置 `NO_COLOR` 时使用普通文本。任意工作目录均可调用。

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
- `TEST_JOBS`、`RIGHT_SEED` / `PERF_SEED`、`TEST_TIMEOUT` / `COMPILE_TIMEOUT`、`FORCE_RIGHT`、`PERF_SIZES`：测试参数。
- 文档、缓存、报告、语雀 CLI 版本与请求超时等路径和设置。

配置文件只保存 Token 的路径。Token 值单独保存在忽略的 `.yuque/token`，权限 600，或使用 `YUQUE_TOKEN` 环境变量。

## 更新本地 snippets

```bash
ctl update --local
```

自动查找 macOS、Windows、Linux 的默认 VS Code / Insiders 用户配置、Profiles、便携版与 VS Code Server；WSL 中也会尝试寻找 Windows 配置。多个候选时停止并列出路径，需要在 init 中指定。

只维护 `acm-templates.code-snippets`，个人 snippets 和 `init_`、`init_case` 保留。重复前缀或无法解析的文件会阻止安装。旧模板迁移及受管理文件更新前备份到 `.cache/snippet-backups/`。

在 C++ 编辑器输入 `_T_模块名`。RMQ 同时支持 `_T_RMQ`、`_T_SparseTable`。
Snippets 只包含模板正文，前置模板按清单和模块 README 插入。

## 更新语雀

正文、标题和排列顺序维护在根目录 `Library.md`；模板代码维护在 `src/`。用独占一行的标记引用源码：

```markdown
### FenwickTree

下标从 0 开始，query(i) 包含 i。

<!-- @code FenwickTree -->
```

Geo2 使用 `Geo2/PointVec` 等分层名；模块路径和依赖在 `Manifest.json`。每个模板必须引用一次。普通代码围栏用于补充说明，不进入 snippets；`<!-- @tool CTL -->` 展示当前命令入口。

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

正确性使用严格调试、优化、ASan/UBSan 三种配置，覆盖各模块和组合测试；全库范围还运行工具回归。缓存必须匹配源码、依赖、编译器和参数。性能先并行编译，再顺序测量，两档规模、两种形状，每组预热一次、采样七次。

`ctl test --all` 会执行两类测试，即使正确性失败也继续收集性能结果；最终返回失败并保留各自报告。各模式结束后均汇总 `.cache/reports/Verification.md`，部分测试通过不代表完整发布验证通过。

测试执行器目前需要 macOS 或 Linux；Windows 可使用 WSL。详细覆盖与边界见 [tests/README.md](../tests/README.md)。

内部模块分别负责清单、文档、snippets、语雀客户端和测试执行，均由 `ctl` 调用；旧命令及导出、诊断、列表、独立认证、测试文档等入口已移除。
