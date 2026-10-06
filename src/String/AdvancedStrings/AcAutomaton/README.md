# AcAutomaton

[code.hpp](code.hpp)

`AcAutomaton<D=26,Base='a'>` 只维护 son/link/order，根为 0，节点数为 son.size()。

`add(p,c)` 扩展 Trie，`add(text,fn)` 从根插入并返回终点；完整模式贡献只在最后一个字符登记，空模式由返回的根单独登记。`build()` 补全失败转移，可重复调用，此后不能插入。`step(p,c)` 在 build 后进行 O(1) 转移。

order 为原始 Trie 的 BFS 顺序，link 父亲在前；不是完成转移图的拓扑序，也不按 link 树深度排序。逆 order 汇总次数，正 order 继承失败链权值，均跳过根。查询文本各自从根开始，空模式匹配 |text|+1 个边界。

字符须在 unsigned(Base) 起点、宽度 D 的连续字节范围内；任意字节用 D=256、Base=0。构造参数只预留容量，节点编号与长度须适合 int。

整串回调 `fn(p,i)` 在每个字符的结构更新完成后执行，i 从本次字符串的 0 开始；复用节点也回调，空串不回调。闭包只在调用期间借用，不保存；不能重入修改或借用扩容时会失效的内部存储。外部数组按完整节点数 resize，新增槽位为零贡献；保留直接贡献，用副本汇总，追加后重新汇总。

V 个节点构建 O(DV)、空间 O(DV)，插入成本另加字符数量；回调成本另计。

[使用示例](../../../../tests/Correctness/String/AdvancedStrings/AcAutomaton/Demo.cpp)。
