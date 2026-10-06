# PalindromeAutomaton

[code.hpp](code.hpp)

`Pam<D=26,Base='a'>` 只维护 son/link/len/s/cur。0 为长度 0 根，1 为长度 -1 虚根，真实回文从 2 起，link[0]=link[1]=1。

`add(c)` 返回当前最长回文后缀节点；`add(text,fn)` 继续已有文本，累计位置用追加前 s.size()+i。s 拥有文本，输入 view 不能借用本对象的 s。

一次追加至多新增一个节点，没有 clone。link 指向更早创建的节点，按编号逆序从 2 起向 link 汇总即可；两个根不计真实回文答案，不合并虚根的自 link。次数、位置、后缀数量等全部由外部数组维护。

字符须在 unsigned(Base) 起点、宽度 D 的连续字节范围内；任意字节用 D=256、Base=0。构造参数只预留容量，节点编号与长度须适合 int。

整串回调 `fn(p,i)` 在每个字符的结构更新完成后执行，i 从本次字符串的 0 开始；复用节点也回调，空串不回调。闭包只在调用期间借用，不保存；不能重入修改或借用扩容时会失效的内部存储。外部数组按完整节点数 resize，新增槽位为零贡献；保留直接贡献，用副本汇总，追加后重新汇总。

n 个字符构建 O(Dn)，空间 O(DV+n)，回调成本另计。

[使用示例](../../../../tests/Correctness/String/AdvancedStrings/PalindromeAutomaton/Demo.cpp)。
