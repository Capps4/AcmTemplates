# ExSuffixAutomaton

[code.hpp](code.hpp)

`ExSam<D=26,Base='a'>` 只维护 son/link/len，根为 0、link[0]=-1，节点数取 len.size()。

`add(p,c)` 从已有前缀扩展；`add(text,fn)` 每串从根开始并返回终点，避免跨源串子串。返回节点可能新建、复用或拆分；纯结构 clone 无输入贡献，拆分节点若正是当前实际前缀，则回调登记一次贡献。

`find(text)` 返回 optional，缺失为 nullopt、空串为有值的根 0。`match(p,l,c)` 在固定结构上维护最长匹配后缀，初始 p=l=0；修改结构或更换查询后重新扫描。

`getOrder()` 为 len 升序，也是 son DAG 拓扑序，link 父亲在前但不保证 link 树深度递增。逆序汇总次数/来源集合，跳过根；不同子串数为 Σ(len[p]-len[link[p]])，根汇总不是空串次数。

字符须在 unsigned(Base) 起点、宽度 D 的连续字节范围内；任意字节用 D=256、Base=0。构造参数只预留容量，节点编号与长度须适合 int。

整串回调 `fn(p,i)` 在每个字符的结构更新完成后执行，i 从本次字符串的 0 开始；复用节点也回调，空串不回调。闭包只在调用期间借用，不保存；不能重入修改或借用扩容时会失效的内部存储。外部数组按完整节点数 resize，新增槽位为零贡献；保留直接贡献，用副本汇总，追加后重新汇总。

L 次扩展、最长前缀 M，V≤2L+1，空间 O(DV)，单次扩展最坏 O(M+D)，getOrder O(V+M)，find 和固定结构的完整流式查询 O(|text|)。任意前缀扩展的总成本不直接承诺线性。

[使用示例](../../../../tests/Correctness/String/AdvancedStrings/ExSuffixAutomaton/Demo.cpp)。
