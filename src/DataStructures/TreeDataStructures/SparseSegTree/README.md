# SparseSegTree

[code.hpp](code.hpp)

`SparseSegTree<Info,Coord>` 使用共享整数域与静态节点池，先调用 `clearInit(lo,up,cap=0)`，域为非空 `[lo,up)`，up-lo 须可由 Coord 表示。再次初始化使旧树和视图失效，池只增长。

`modify(x,info或updater)` 更新叶子；updater 为 `Info(const Info&)`。`merge(b,leafRule)` 合并到当前树，普通 b 保留，move(b) 消费；自定义规则为 `Info(const Info&,const Info&,Coord)`，须以 Info{} 为双侧单位元。

`query(l,r)` 查询非空 `[l,r)`；`first/last(l,r,f)` 分别按前缀/后缀累计搜索，返回 optional 坐标，空范围或无解为 nullopt，负坐标也可为答案。谓词随范围扩展不能由 true 变 false；计数选择须保证组合后的计数非负。

复制根自动 COW 保护快照，导出 `a+b`、`a-b` 等 View 也保存根快照；修改不影响旧视图。Info 可默认构造/复制，Info{} 为汇总单位元，加号有序结合；减法视图才要求减号。最大/最小值不能直接相减恢复集合差。

点修改、单根查询/搜索 O(log U)，M 根视图 O(M log U)。合并遍历同时存在的部分，共享节点可能复制，不能直接套用破坏性合并的均摊界。没有 lazy 下推；回调不得重入修改同池树，引用不可跨扩容保留。
