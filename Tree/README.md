# Tree

保留原来按 Stage/Attribute 组合的树框架，使用 C++17 fold expression、类型 traits 和 constexpr 索引。FullTree/LcaTree/SimpleTree 别名保留；内部命名空间恢复 Original 的 `_tree`。

每个 Stage 改用显式 12 字节 DFS 帧，保持 enter、leave、afterChild 的原来顺序以及同 Stage 的属性顺序。二十万节点链无需增加系统栈。框架借用非 const 邻接表，调用方保证它存活；Heavy 会交换整条邻接边，将重儿子放到第一项，可使用 move-only 权重。输入是连通无环的无向树，顶点 0index；非空时 root 有效，空树 root 为 -1、数组为空。构建后不应修改邻接表。dist 仍为边数，不是权重和。

getRoad 返回 `vector<_tree::PathSegment>`，按照 x 到 y 顺序，每段为 `[l,r)` DFN 区间，reversed 表示从 r-1 向 l 遍历。旧版闭区间/向上端点 pair 接口需要迁移；LCA 只包含一次。kthAncestor 的 k 非负、超过根深度返回 -1。子树为 `[dfn[x], dfn[x]+size[x])`。getVirtualTree 去重 keys/extra 并加入必要 LCA；dfsOrder 为原图 ID，fa 为虚树局部 0index，根 fa=-1，inputIndex 是 keys 第一次出现的位置（额外点为 -1）。空输入返回空虚树。

随机树与独立 BFS/父链 oracle 对照 fa/dep/size、全部祖先关系、kthAncestor、LCA、距离、路径完整顺序和虚树的 LCA 闭包/最近父节点。另测准确 hook 顺序、空树、任意根、带 move-only 权重和二十万链。GCC -O2 与 Clang ASan/UBSan 通过。

构建 O(n) 每 Stage，LCA/kthAncestor/getRoad O(log n)，虚树 O(k log k + k log n)。显式帧增加堆内存和状态分支；本机旧/新链、星形、随机树构建及 LCA 查询均约回退 10%，不宣称提速。换取深树可用性和明确半开接口。原始七轮计时和命令在 Results.json，当前中位数见 Performance.md。风格：类型大驼峰、方法/变量小驼峰、0index、半开区间；不增添自动依赖排序或额外属性存储。

本轮以 Original 为基准：保留 Original 属性/Stage/Ability 结构，不引入额外依赖框架。仅 Stage DFS 用显式栈保障长链；删除不用 visit 与 checkedSize，命名空间恢复 _tree。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check Tree`；性能用 `python3 template/Run.py bench Tree`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
