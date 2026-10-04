# AC 自动机 / GNU++17

模式输入支持 string/string_view 容器及字面量列表。长度统计改用 const 引用，避免旧版复制每个模式；Trie 节点分块初始化，避免重复模式把整张最坏容量表清零。build 用连续 BFS order 替代 queue，保留 son/link/tot，新增 terminal 保存每个模式的终点（重复模式映射到同一节点）。构造后 son/link 只有有效节点，tot 为最后一个节点下标，根为 0。

countOccurrences(text) 对状态访问次数沿失败链接逆 BFS 聚合，返回与模式输入顺序相同的重叠匹配次数。空模式匹配 text.size()+1 个边界，空模式集合得到空结果。模式及查询文本都必须位于 [Base,Base+Z) 的字节范围；Z>0 且 Base 的 unsigned 值加 Z<=256。Z=256/Base=0 支持所有字节与 NUL。总模式长度<=INT_MAX-1，计数用 long long，文本长度<LLONG_MAX。输入只在构造期间借用，之后不保留 view。构造 O(Z*节点数+总长度)，查询 O(text长度+节点数+模式数)，空间 O(Z*节点数)，reserve 仍保留最坏容量。

0index，匹配次数对应各完整模式，底层 transition 是构建后的完整转移而非纯 Trie；不能在 build 后直接新增模式或破坏公开数组。类名 AcAutomaton 大驼峰，方法/变量小驼峰。

测试：二进制短模式对与文本穷举，直接逐位置比对重叠出现次数；重复模式、空模式/集合/文本、256 字节、string_view、字面量列表、百万重复字符。GCC -O2 与 Clang ASan/UBSan 均通过。

自评：保留核心失败转移，终点映射使模板可以直接获取匹配次数；新增字段均服务构造/查询。短/长随机模式基本持平，长模式仍约 6% 回退；重复模式避免未使用表初始化，约 3.8x。本轮逐节点 push 方案在短模式明显变慢，最终选分块初始化。完整测量在 Benchmark.csv，未用错误或越界输入证明加速。

源码、依赖和运行结果由统一入口记录于 Reports；目录中的 Results.json 为历史报告。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

本轮以 Original 为基准：Original trie/fail 构建保留；unsigned byte 编码与按块初始化修复字节问题并减少重复模式初始化。保留常用计数接口，删除不可达容量检查。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check AcAutomaton`；性能用 `python3 template/Run.py bench AcAutomaton`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
