# Sam / GNU++17

支持 string_view；默认/空输入仍有根节点，修复旧 Sam(0) 的 link[0] 越界。expectedLength 只用于 reserve，可从不足容量继续追加，std::vector 根据实际节点增长。新增 add(char) 返回新末状态；兼容 add(id,char)，原先未使用的 id 仍不自动登记。新节点与 clone 由小助手统一创建，保留标准后缀链接拆分逻辑和公开 son/link/len/last/tot。

contains(text) 查询子串，空文本返回 true；distinctSubstringCount() 用 sum(len[v]-len[link[v]]) 返回非空不同子串数。所有字符必须在 [Base,Base+Z) 字节范围；Z>0、unsigned(Base)+Z<=256。Z=256/Base=0 支持任意字节及 NUL。字符串视图只在构造时读取，生命周期无需延续到对象之后。根为 0，tot 为最后状态 ID，数组前缀 [0,tot+1) 有效，0index。预计长度非负且<= (INT_MAX-1)/2；动态节点下标和长度仍需适合 int，实际受内存限制。

构造/连续追加的总算法工作量 O(Z*n)（含 clone 的固定字母表拷贝），contains O(查询长度)，计数 O(状态数)，内存 O(Z*状态数)，reserve 容量可能大于实际状态数。公开数组不可任意修改，否则状态不变量会失效。Sam 为大驼峰，方法/变量小驼峰；保留简短既有类名。

测试：0..10 位二进制字符串穷举，set 枚举非空子串作为独立 oracle；每次 online add 后验证前缀语言和不同子串数；随机候选词、克隆/失败链接长度不变量、任意字节、空输入、从 expectedLength=1 追加 100000 字符、临时 string。GCC -O2、Clang ASan/UBSan 均通过。

自评：移除空的 assign 回调，不增加未请求的出现次数字段；需要登记前缀可使用 add 的返回状态。100000 字符 alphabet 1/2/26 构造约 1.28x/1.08x/1.27x；以完整状态长度与链接校验和对照旧版，数据见 Benchmark.csv。

源码、依赖和运行结果由统一入口记录于 Reports；目录中的 Results.json 为历史报告。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

本轮以 Original 为基准：Original add/clone/len/link 保留；追加增长补空输入及容量安全；删除 checkedLength/极大容量 assert。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check SuffixAutomaton`；性能用 `python3 template/Run.py bench SuffixAutomaton`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
