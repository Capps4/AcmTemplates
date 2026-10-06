[语雀模板库](https://www.yuque.com/capps/template/library)

# ACM Templates

一份持续整理的 C++17 竞赛算法模板库，涵盖数据结构、图论、树论、字符串、数学、几何、排序与动态规划。

**源码、说明、测试一起维护。** 语雀文档与 VS Code snippets 由仓库内容生成。

## 使用模板

按分类浏览 [src/](src/)，每个模块的 README 说明用法、边界与复杂度；现用实现通常在 `code.hpp`，Geo2 按几何层次拆分。

```cpp
#include "src/DataStructures/TreeDataStructures/FenwickTree/code.hpp"
```

也可以安装到 VS Code，在 C++ 文件中输入 `_T_模块名` 插入模板；有前置依赖时按模块说明一并插入。

## 常用命令

`ctl` 是 Capps Template Library 的统一入口，需要 Python 3.9+。安装方式见 [工具说明](tools/README.md)；未安装时可在仓库根目录使用 `./ctl`。

```bash
ctl --help          # 查看帮助
ctl update --local  # 自动定位并安装 VS Code snippets
ctl update --yuque  # 生成并更新语雀文档
ctl rule            # 检查模板规则
ctl test --right    # 正确性测试
ctl test --perf     # 性能测试
ctl test --all      # 两种测试都执行
```

统一配置在 [tools/__init__.py](tools/__init__.py)。语雀更新需要 Token 和有效的全库测试记录；Token 单独保存在忽略的 `.yuque/token`，或通过 `YUQUE_TOKEN` 环境变量提供。

## 仓库结构

| 位置 | 内容 |
| --- | --- |
| [src/](src/) | 算法模板与模块说明 |
| [Headers/](Headers/) | GCC / Clang 共用的标准头入口 |
| [Library.md](Library.md) | 语雀正文、分层与源码引用 |
| [Manifest.json](Manifest.json) | 模块路径、分层及依赖清单 |
| [tests/](tests/) | 正确性、性能与工具回归用例 |
| [tools/](tools/) | CTL 的内部实现与配置 |

生成文件、构建产物与报告保存在忽略的 `.cache/`。

[工具说明](tools/README.md) · [测试说明](tests/README.md) · [实现规则](Docs/Rules.md)
