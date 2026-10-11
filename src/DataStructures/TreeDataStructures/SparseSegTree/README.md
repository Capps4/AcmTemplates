# SparseSegTree

[code.hpp](code.hpp)

`SparseSegTree<Info, Coord, Tag = void>` 使用共享整数域与静态节点池，先调用 `clearInit(lo, up, cap = 0)`，域为非空 `[lo, up)`，`up - lo` 须可由 Coord 表示。cap 是预留节点数，不是上限；不足时自动扩容。再次初始化使旧树和视图失效，节点池只增长。

`modify(x, info或updater)` 更新叶子；updater 为 `Info(const Info&)`，返回修改后的实际值，已包含祖先贡献。提供 Tag 时，`modify(l, r, tag)` 修改 `[l, r)`，空区间无操作；完全覆盖只修改当前节点，永久标记不下推。

Tag 只需默认构造和 `void apply(const Tag&)`，Tag{} 为恒等操作。Info 需要默认构造、复制及 `operator+`；带标记点修改和减法视图需要 `operator-`。启用 Tag 时统一提供 `void apply(const Tag&, Coord l, Coord r)`；不使用边界也保留两个坐标参数，可以不写参数名。恒等 Tag 同样会调用 apply，用于填写区间元信息。

永久标记仅支持可分解、可交换的可加贡献：数据须满足 `apply(v, t) = v + apply(Info{}, t)`，并与逐点相加合并相容；不能直接换成任意赋值、乘法或仿射标记。Info{} 为零数据，Info 的加号可同时累加相邻区间的长度。多根视图组合的是同一区间：树在每次加减前通过 `apply(Tag{}, l, l)` 清空累计区间元信息，保留数据，最后回填真实 `[l, r)`；避免长度重复累计或中间溢出。apply 必须支持空区间，恒等 Tag 只更新元信息，不改变数据。

末尾示例为全局 Info/Tag，无树或整数别名。`SparseSegTree<Info, long long, Tag>` 的 `query(l, r).sum` 返回区间和，`.len` 返回 `r - l`，空节点与差分视图的长度也正确。示例使用 int len，整个域长须不超过 INT_MAX；sum、Tag 累加及乘法须不溢出 long long。大坐标域可自定义更宽的 Info 字段。直接在 apply 的 l/r 参数中取得左右端点，需要保存时自行加字段；点修改和搜索回调统一只接收 Info，需要边界时从 Info 的自定义字段读取。Tag=void 时不会调用 apply，因此示例 len 的自动填写需要启用 Tag。

`merge(b)` 合并到当前树，普通 b 保留，move(b) 消费，带标记时按逐点相加合并。Tag=void 时还支持 `merge(b, leafRule)`；规则为 `Info(const Info&, const Info&, Coord)`，须以 Info{} 为双侧单位元。

`query(l, r)` 查询非空 `[l, r)`；`first/last(l, r, f)` 按前缀/后缀累计搜索，返回 optional 坐标，空范围或无解为 nullopt，负坐标也可为答案。谓词为 `bool(const Info&)`，随范围扩展不能由 true 变 false；计数选择须保证组合后的计数非负。

复制根自动 COW 保护快照，导出 `a + b`、`a - b` 等 View 也保存根快照；修改不影响旧视图。加号有序结合；最大/最小值不能直接相减恢复集合差。节点 info 包含本节点及后代贡献，不包含祖先贡献；叶子的 Tag 恒为空。

点/区间修改、单根查询/搜索 O(log U)，M 根视图 O(M log U)，U 为域长。合并遍历同时存在的部分，共享节点可能复制，不能直接套用破坏性合并的均摊界。没有 lazy 下推；回调不得重入修改同池树，引用不可跨扩容保留。
