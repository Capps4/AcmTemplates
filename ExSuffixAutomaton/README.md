# ExSam / GNU++17

支持 string/string_view 容器及字面量列表；默认空集合和全空字符串也有根，修复旧版 link[0] 越界。每个字符串从根开始扩展，保留已有转移需要拆分 clone 的逻辑，避免把不同字符串拼接成额外子串。新增 addString(text)、contains(text)、distinctSubstringCount()；exadd(char,id) 仍用于当前 last 的扩展，返回当前状态，id 与旧版一样未自动登记。

根为 0，tot 是最后状态 ID，son/link/len 的有效状态为 [0,tot+1)，0index。预计总长度先统计后 reserve 2*sum+1，只初始化实际产生的节点；超出预计容量时 vector 正常增长。输入不保留视图。所有字符须在 [Base,Base+Z) 的字节范围；Z>0 且 unsigned(Base)+Z<=256，Z=256/Base=0 支持 NUL/任意字节。预计总长度<= (INT_MAX-1)/2，所有实际节点编号/长度仍需适合 int，规模受内存限制。公开数组/last 的手动修改须保持不变量；开始新字符串应调用 addString，直接 exadd 会继续当前路径。

contains 空串返回 true，不同子串数排除空串；计数采用 sum(len[v]-len[link[v]])。构造工作量 O(Z*总字符数)（包括固定字母表 clone 拷贝），contains O(查询长度)，计数 O(状态数)，空间 O(Z*状态数)，reserve 仍可能保留比实际状态更多的容量。ExSam 大驼峰，方法/变量小驼峰。

测试：二进制长度<=4 的所有字符串对（含重复串），set 枚举子串并集作为独立 oracle；每次 addString 后检验语言，随机候选词、边界不跨串、空集合、全空串、字面量列表、string_view、完整字节字母表、从空对象追加 200000 长字符串、失败链接长度不变量。GCC -O2 与 Clang ASan/UBSan 均通过。

性能评估：1000 个 100 字符字符串，alphabet 1/2/26 最新约 1.13x/0.92x/1.14x。二进制场景约 8% 回退，来自动态节点操作/检查等成本，不能宣称所有场景更快。尝试分块清零与整表初始化后都更慢，最终保留更简单的按需 push 方案。GCC 原本将热 newNode 帮手独立为函数；使用 GNU/Clang [[gnu::always_inline]] 将该小助手内联后减小回退，正确性不依赖该优化属性。这是 GNU++17 的编译器属性，不是 ISO C++17 的语言功能。原版只比较非空合法字母串，完整状态长度差校验和相同，数据见 Benchmark.csv。

自评：保留广义后缀自动机完整语言，不改成仅逐串独立索引；原空 assign 钩子移除，调用者可用返回状态自行登记。多场景性能权衡与根/生命周期契约写明。当前源码和依赖的验证状态以统一入口生成的 Reports 为准。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

本轮以 Original 为基准：保留 Original 两种 exadd/clone 分支；根哨兵和实际节点增长修复空输入/重复串空间。删除容量 guard，短名与展开分支。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check ExSuffixAutomaton`；性能用 `python3 template/Run.py bench ExSuffixAutomaton`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
