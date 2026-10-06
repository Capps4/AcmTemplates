# SegTree

[code.hpp](code.hpp)

`SegTree<Info,Tag=void>` 为迭代线段树，下标从 0 开始，区间 `[l,r)`。`modify(p,value)` 点赋值，`query(l,r)` 汇总；有 Tag 时支持 `modify(l,r,tag)`。

Info 可从输入构造、复制和赋值，`operator+` 满足结合律并保留左右顺序，不要求单位元或默认构造；构建只合并真实叶子。Tag() 为空操作，`Info::apply(tag)` 更新汇总，`Tag::apply(after)` 表示先旧后新，须与合并兼容。末尾 Info/Tag 是按题目替换的示例。

query 要求非空合法区间；空树允许构造，空修改无操作。`first/last(l,r,pred)` 返回首/末匹配叶子的 optional 下标，无解或空范围为 nullopt，0 是有效答案。pred 对节点判假时须保证整个节点无匹配叶子；此搜索不累计前缀。

构建与空间 O(base)，base 为补齐二次幂；点/区间操作 O(log n)。精确存在性谓词的搜索 O(log n)，允许假阳性的剪枝谓词最坏 O(n)。Info/Tag/谓词成本另计，n≤INT_MAX/2。
