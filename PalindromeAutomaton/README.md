# Pam / GNU++17

string_view 构造，expectedLength 仅为预计容量；根始终存在，可从小容量继续 add。保留公开 son/link/len/dep/cnt/s/cur/tot。新增 add(char) 返回最长回文后缀状态，兼容 add(i,char) 并断言 i 等于当前字符数。s 保存原始字节（旧版保存 c-Base 的编码），转移单独计算字母编号，支持完整 256 字节范围。

保留双根：0 代表空回文，1 代表长度 -1 的虚根；link[0]=1，link[1]=1，失败树仍以 1 为根。实际回文节点从 2 起，tot 为最后 ID，底层数组 0index。dep 是该状态失败链中的非空回文数；cnt 仍只累计其作为最长后缀的次数。occurrences() 返回新数组，将计数向失败链接聚合，不修改 cnt，可重复调用；真实回文状态的值为全文出现次数。根的计数不代表普通子串出现次数。

字节要求在 [Base,Base+Z)，Z>0 且 unsigned(Base)+Z<=256；Z=256/Base=0 支持 NUL。长度与节点需适合 int，预计长度<=INT_MAX-2，当前文本长度<INT_MAX-2。输入视图只在构造时读取，之后 s 自己持有文本。公开状态可用于算法扩展，但修改必须保持不变量。构造 O(Z*n)，计数/失败树 O(节点数)，空间 O(Z*节点数+n)。Pam 大驼峰，方法/变量小驼峰。

测试：0..11 二进制字符串穷举，与 map 中枚举的回文及频率对照；每个前缀直接数回文后缀验证 dep；失败树入度/长度不变量，1000 个随机字节串、NUL、空输入、100000 次超预计追加、临时 string。GCC -O2 与 Clang ASan/UBSan 均通过。

自评：保留直接 cnt 含义，避免聚合后再次 add 的计数污染。性能 alphabet 1/2/26 约 1.34x/1.09x/1.11x。基准仅对非根状态求长度/链接校验和，因虚根的 link 明确改为自环；全部真实回文语义由独立 oracle 验证。详细数据在 Benchmark.csv。

源码、依赖和运行结果由统一入口记录于 Reports；目录中的 Results.json 为历史报告。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

本轮以 Original 为基准：Original 两根/失配链接/后缀计数保留；string_view、追加时增长和半开实际节点数组，删除容量 helper。短 occurrences 使用现成 cnt/link，不加常驻状态。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check PalindromeAutomaton`；性能用 `python3 template/Run.py bench PalindromeAutomaton`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
