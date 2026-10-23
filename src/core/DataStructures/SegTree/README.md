# SegTree

[code.hpp](code.hpp)

`SegTree<Info,Tag=void>` 为迭代线段树，下标从 0 开始，区间 `[l,r)`。`modify(p,value)` 点赋值，`query(l,r)` 汇总；有 Tag 时支持 `modify(l,r,tag)`。

Info 可从输入构造、复制和赋值，`operator+` 满足结合律并保留左右顺序，不要求单位元或默认构造；构建只合并真实叶子。Tag() 为空操作，`Info::apply(tag)` 更新汇总，`Tag::apply(after)` 表示先旧后新，须与合并兼容。末尾 Info/Tag 是按题目替换的示例。

query 要求非空合法区间；空树允许构造，空修改无操作。`first/last(l,r,pred)` 与 SparseSegTree 一致，按前缀/后缀累计搜索：first 返回最小 i，使 `pred(query(l,i+1))` 为 true；last 返回最大 i，使 `pred(query(i,r))` 为 true。结果为 optional 下标，无解或空范围为 nullopt，0 是有效答案。

pred 接收 `const Info&`，随区间向搜索方向扩展不能由 true 变 false；不对空累计调用谓词，也不要求单位元。非负区间和用 `x.val >= k` 搜索累计达到 k 的位置；非负计数用同样条件查第 k 个元素。维护区间最大值时，用 `x.mx >= k` 仍能查首/末个值至少为 k 的元素。末尾示例维护区间和，搜索要求数据非负；它不再表示单个叶子的值达到阈值。

构建与空间 O(base)，base 为补齐二次幂；点/区间操作及单调累计搜索 O(log n)。搜索只下降一条命中路径，并通过 optional 累计跳过的整块，保留左右合并顺序。Info/Tag/谓词成本另计，n≤INT_MAX/2。
