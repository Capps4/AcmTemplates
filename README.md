# ACM Templates

统一维护的 C++17 竞赛模板库：**core 用于常用模板与 VS Code Snippet，extra 用于专题和扩展模板。两者共用规范、依赖管理、正确性和性能验收。**

[语雀 core](https://www.yuque.com/capps/template/library) · [语雀 extra](https://www.yuque.com/capps/template/library_extra)

## 使用

按 [src/core/](src/core/) 和 [src/extra/](src/extra/) 浏览模块；每个 README 说明支持域、索引、区间、复杂度与依赖。

```cpp
#include "src/core/DataStructures/RangeQuery/FenwickTree/code.hpp"
```

core 的 VS Code 前缀保持 `_T_模块名`；已有兼容前缀继续保留。extra 不生成 Snippet。迁出的 11 项会从受管理 Snippet 集合移除，旧独立模板文件先备份再迁移；个人 Snippet 和初始化模板保留。

## 命令

`ctl` 需要 Python 3.9+；未安装时在仓库根目录使用 `./ctl`。

```bash
ctl update --local
ctl update --yuque --collection core
ctl update --yuque --collection extra
ctl rule
ctl test --all
ctl test --all --collection core
ctl test --all --collection extra
ctl test --right --module Tree --case chain --profile optimized
```

规则检查和未指定集合的测试覆盖全仓；指定集合的测试包括传递依赖，并保留共享工具回归。语雀发布检查对应集合及依赖的现版正确性、性能和工具证据，不自动启动测试。配置在 [tools/__init__.py](tools/__init__.py)，凭据只放忽略的 `.yuque/token` 或环境变量。

## 结构

| 路径 | 职责 |
| --- | --- |
| [src/core/](src/core/) | 42 个核心模块，生成 Snippet |
| [src/extra/](src/extra/) | 31 个扩展模块，不生成 Snippet |
| [Headers/](Headers/) | 共用标准头入口 |
| [Manifest.json](Manifest.json) | 全仓唯一模块清单、集合、依赖与分层 |
| [Library.md](Library.md) / [ExtraLibrary.md](ExtraLibrary.md) | 两份语雀文档的正文与源码引用 |
| [tests/](tests/) | 按源码相对 src 的路径镜像的正确性、性能测试及共享支持 |
| [tools/](tools/) | 共用 CTL 执行器与生成/发布逻辑 |
| [Docs/](Docs/) | 实现规范及 extra 迁移映射 |

core 的目录与语雀按以下七类组织；包含多个模块的明确主题增加一层目录，其他模块直接放在大类下。

| 分类 | 主题与模块 |
| --- | --- |
| Math | Numeric、NumberTheory；Combinatorics、NumberTheoreticTransform、LinearBasis |
| DynamicProgramming | MultipleBackpacks |
| String | StringHash、Kmp、ZFunction、Manacher、AcAutomaton、SuffixAutomaton、SuffixArray |
| DataStructures | Map、RangeQuery；DisjointSetUnion、Trie |
| Graph | Trees、Connectivity、Flow；Dijkstra、TopSort、GridUtil |
| Geometry | Geo2 |
| Utilities | ListHelper、Discreter、Debuger、DebugerHeadCopyToBitsStdC++、FastInputOutput |

extra 保留现有分类。core 只能依赖 core，extra 可以依赖两套模板，依赖必须无环。示例与伪代码保留在 extra 文档的 Examples and Notes，不计入可编译模块或测试通过数。

源码、说明、测试一起维护，Git 是唯一内容源；语雀代码卡片与 Snippet 从仓库生成。产物、报告放忽略的 `.cache/`，语雀备份与发布状态放忽略的 `.yuque/`。

[工具说明](tools/README.md) · [测试说明](tests/README.md) · [实现规则](Docs/Rules.md) · [extra 迁移记录](Docs/ExtraMigration.md)
