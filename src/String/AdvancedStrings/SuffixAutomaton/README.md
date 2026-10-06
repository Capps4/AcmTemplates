# SuffixAutomaton

[code.hpp](code.hpp)

`Sam<D=26,Base='a'>` 只维护 son/link/len/last，根为 0、link[0]=-1，节点数取 len.size()。

`add(c)` 继续文本并返回新末状态；`add(text,fn)` 逐字符继续追加。单纯 clone 复制结构，外部出现贡献为零；resize 必须按 len.size()，不能只按返回节点 p+1。

`find(text)` 返回 optional 节点，缺失为 nullopt，空串返回有值的根 0；非空长度位于 `(len[link[p]],len[p]]`。`match(p,l,c)` 在固定结构上维护最长匹配后缀，初始 p=l=0，l 是实际匹配长度。追加后重新扫描。

`getOrder()` 为 len 升序，含根，也是 son DAG 的拓扑序；link 父亲在前，不保证 link 树深度递增。逆序汇总至 link，跳过根；不同非空子串数可由 Σ(len[p]-len[link[p]]) 得到，根汇总不表示空串次数。

字符须在 unsigned(Base) 起点、宽度 D 的连续字节范围内；任意字节用 D=256、Base=0。构造参数只预留容量，节点编号与长度须适合 int。

整串回调 `fn(p,i)` 在每个字符的结构更新完成后执行，i 从本次字符串的 0 开始；复用节点也回调，空串不回调。闭包只在调用期间借用，不保存；不能重入修改或借用扩容时会失效的内部存储。外部数组按完整节点数 resize，新增槽位为零贡献；保留直接贡献，用副本汇总，追加后重新汇总。

n 个字符构建 O(Dn)，空间 O(DV)，getOrder O(V+n)，find 及一次完整流式匹配 O(|text|)，回调成本另计。

[使用示例](../../../../tests/Correctness/String/AdvancedStrings/SuffixAutomaton/Demo.cpp)。
