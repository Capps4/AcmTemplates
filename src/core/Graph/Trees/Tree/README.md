# Tree

[code.hpp](code.hpp)

树属性按分阶段能力组合，`FullTree` / `LcaTree` 借用并重排邻接表以安排重儿子。顶点从 0 开始，树须连通无环；空树 `root=-1`，不能对空树调用节点查询。邻接表的生命周期须覆盖对象。

`dep[x]` 始终是根到 x 的边数，`dist(x,y)` 始终是路径边数。`FullTree<T,W>` / `LcaTree<T,W>` 另提供 `wtDep[x]`（根路径边权和，根为 0）和 `wtDist(x,y)`（唯一路径边权和，允许负边权）。边权固定；构建后修改权值不会自动更新累计值。默认累计类型 W 为 `common_type_t<long long, 边权类型>`，因此 int 边权用 long long，浮点边权保留浮点；可显式指定累计类型。W 须支持零初始化、从边权转换、加减，所有中间结果和答案须可表示；浮点累计遵循所选类型的舍入误差。自定义数值类可显式指定 W；根占位边的 int 权值也须可转换为 W。非数值载荷默认不启用 wtDep/wtDist，原有拓扑能力仍可使用；显式 W=void 也可关闭加权属性与能力。`SimpleTree` 仅保留 fa/dep/size。

提供 LCA、祖先、距离、虚树及路径接口。`getRoad(x,y)` 返回包含两端的 pair：l≤r 正向，l>r 反向；转半开区间时须自行处理方向。将边权存到较深节点对应位置后，边路径查询须排除 LCA 位置。

## 边访问与属性扩展

`_tree::endOf(int)` 返回节点，`_tree::wtOf(int)` 返回单位边权 1；pair<int,W> 的两个重载分别返回 first 和 second，权值按 const 引用访问，不复制载荷。自定义 Edge 在其所属命名空间提供 endOf/wtOf，可通过 ADL 接入。

`TreeAttribute` 保留 `init(n)`，遍历仅有以下两个钩子，均按 Stage 内属性声明顺序调用：

```cpp
template <class Core, class Edge>
void enter(Core& core, int pt, const Edge& e);

template <class Core, class Edge>
void leave(Core& core, int pt, const Edge& e);
```

pt 是父节点，e 是父节点指向当前节点的入边，`endOf(e)` 是当前节点。enter 在处理孩子前执行，leave 在全部孩子处理完成后执行；子树汇总在孩子的 leave 中累加到 pt。根调用传 `pt=-1, e=root`，此时 Edge=int，不构造虚假的权值对象，也不将根的 wtOf 值计入路径。自定义钩子应模板化 Edge，并单独处理根的单位元。

钩子不能增删邻接元素或修改端点，不能保存跨重排使用的边引用。Heavy 在当前节点 leave 时扫描全部孩子并将最大子树的边换到首位，等大时保留先遇到的孩子；它不改变当前入边，后续 leave 钩子仍能读取原入边。Heavy 依赖同阶段或更早阶段的 Size；Top、重链 DFN 必须位于 Heavy 之后的阶段。自定义 Attribute 的构建钩子收到 TreeCore，只能依赖已组合进 Stage 的状态；查询 Ability 通过 CRTP 访问最终树类型。

## 复杂度与调用栈

遍历使用直接递归 DFS，每个 Stage 遍历一次，内置树两遍构建 O(n)。属性存储 O(n)，额外调用栈 O(h)；LCA、两种距离、祖先查询和路径分段 O(log n)。不提供原地 reset，重建对象会重新初始化各属性。

本地深链验证使用 tests/Support/TestStack.hpp 的 256 MiB 测试线程栈，覆盖 200000 个节点；这不表示默认 OJ 调用栈也足够。实际可支持深度取决于编译模式与运行环境，算法头本身不调整栈容量。
