# Sparse SegTree

C++17 稀疏树：静态节点池共享值域，对象存根编号与 frozen 边界两个整数，节点只存孩子与 Info。五个核心入口为 modify、merge、query、first、last。写接口更新当前对象，独占节点原地修改，受快照保护的节点自动 COW。没有 LazyTag 或信息下推。只读 View 通过树之间的加减表达式构造，并提供相同的后三个查询接口。

- [Final.hpp](Final.hpp)：模板与统一查询内核。
- [Demo.cpp](Demo.cpp)：历史版本、差分视图、树上路径与原地操作。

```bash
g++ -std=c++17 -O2 -Wall -Wextra Demo.cpp -o /tmp/sparse-demo
/tmp/sparse-demo
```

头文件尾部提供带 `replaceme` 注释的 `Info` 示例，包含 count / sum 和加减合并，可直接改写；Demo 使用独立的 DemoInfo。

## 最小使用

```cpp
using Tree = SparseSegTree<Info, long long>;
Tree::clearInit(0, 1000000000);
Tree a;
a.modify(42, Info{1});
Tree b = a; // 保存历史根，再通过 COW 修改分支。
b.modify(7, [](Info old) { ++old.count; return old; });
auto diff = b - a;
auto res = diff.query(0, 100);
auto x = diff.first(0, 100,
    [](const Info& prefix) { return prefix.count >= 1; }); // optional，答案为 7。
```

模板参数为 Info 与 Coord，均须显式指定。同一个模板实例共享节点池和整数域 `[lo,up)`。clearInit 必须在创建树前调用；可选第三个参数预留节点数。再次初始化会使旧树和视图失效，保留 vector 容量供多组数据复用。域必须非空且 up-lo 可用 Coord 表示。

## 五个入口

| 接口 | 行为 |
| --- | --- |
| `a.modify(x, info或updater)` | 更新 a 的根，返回 a 的引用 |
| `a.merge(b, leafRule=加法)` | 合并到 a，保留 b，返回 a 的引用 |
| `a.query(l,r)` | 返回非空区间 `[l,r)` 的汇总信息，要求 l < r |
| `a.first(l,r,f)` | 返回最小 x，使 f(query(l,x+1)) 成立 |
| `a.last(l,r,f)` | 返回最大 x，使 f(query(x,r)) 成立 |

修改回调为 `Info(const Info&)`，仅在叶子执行；传入 Info 则直接覆盖叶子。父节点总是通过加号重算。

无需指定写入模式。`Tree b=a` 复制根编号，同时把 a、b 的 frozen 设为当前节点池大小，不复制节点；后续修改任一棵树时，受保护的路径自动复制，新节点直接复用。默认合并同位置叶子相加，自定义规则签名为 `Info(const Info&,const Info&,Coord)`。

```cpp
Tree c = a;
c.merge(b); // 保留 a、b，合并结果放在 c。
c.merge(b, leafRule); // 自定义同位置叶子的合并规则。
```

merge 按值接收右树。传入普通对象时保留源树；传入 `std::move(b)` 时转移所有权，b 变为空树，可以复用其独占节点。即使 b 已经有历史版本，共享节点仍受自动 COW 保护。

```cpp
Tree x, y;
x.modify(3, Info{1});
y.modify(8, Info{1});
x.merge(std::move(y)); // 消费 y，独占节点可直接接入。
```

## frozen 与自动 COW

节点池只追加，节点编号就是 vector 下标，0 留给空节点。每棵树维护一个 frozen 边界：非零编号小于 frozen 的节点可能被快照引用，修改时复制；编号大于等于 frozen 的可达节点归当前树独占，可以原地修改。frozen 为 0 表示整棵树独占。

- 复制非空树或导出 View 时，将源树的 frozen 设为当前节点池大小；复制出的树使用同一边界。空树的边界为 0。
- 移动时转移根与边界，并将源树两者清零。
- 普通 query、first、last 借用根，不改变 frozen。
- merge 取两边 frozen 的最大值，让接入分支的历史版本继续受保护。

修改时，独占节点直接复用，遇到受保护节点便转入路径复制内核，省去后续逐层判断。复制后分配的新路径位于边界以外，下一次修改同一路径便可原地复用。整棵树独占时直接进入原地内核，不在每层检查 frozen；内部的 InPlace / Copy 类型只用于选择内核，调用方无需指定。

边界是保守的所有权记录。合并取最大值可能把原本独占的节点也纳入保护范围；转入路径复制内核后也可能复制独占后代。旧版本销毁不会主动降低边界，因此可能多复制一些节点。实现无需节点共享标记、引用计数或析构维护，也不涉及 Info 下推。

## 前缀与后缀搜索

两个搜索返回 `std::optional<Coord>`；无答案和空区间返回 nullopt，因此 -1 也可以是合法坐标。区间均为左闭右开，返回的是叶子坐标，不是半开边界。

f 必须是稳定的判断函数；随着前缀/后缀扩展，不能从 true 变回 false，始终为 true 或 false 也允许。单调性由调用方保证，不做运行时检查。搜索维护累计信息，整块判断失败时跳过，成功时下降。前缀使用 `acc+block`，后缀使用 `block+acc`，保持原有坐标顺序，不要求单树的加号满足交换律。

第 k 小用前缀累计计数 >= k，第 k 大用后缀累计计数 >= k。计数须非负；存在负数时直接对 sum 判断阈值未必单调。最大值达到阈值也适用，可以表达原 Lazy 模板 findFirst/findLast 的最大值定位场景。

## View 的同接口查询

```cpp
auto sum = a + b;
auto diff = a - b;
auto path = version[u] + version[v]
          - version[w] - version[parent[w]];
path.query(l,r);
path.first(l,r,f);
path.last(l,r,f);
```

View 用固定大小数组保存根快照和正负号，不分配树节点，不提供写接口。导出 view 或构造加减表达式会更新源树的 frozen，后续修改不改变旧视图。单树查询借用单根 View 复用查询内核，不改变 frozen；clearInit 则使所有旧视图失效。

多根信息应能逐项线性组合；计数、总和适用，最大值/最小值不能通过相减恢复集合差。含减法的视图才要求 Info 支持减号，单树和纯加法视图无需减号。搜索的单调性必须针对组合后的信息成立，例如路径计数的每个叶子都须非负。

原树 DFS 与 LCA 在模板外。节点路径使用上面的四根表达式；边权路径使用 `version[u]+version[v]-version[w]-version[w]`。视图之间也支持加减和括号组合。

## Info 与性能约定

Info{} 为汇总单位元，空节点的信息为 Info{}；未分配的位置没有贡献。query 要求区间非空，递归只访问与查询区间相交的分支。operator+ 有序且满足结合律，Info 需默认构造和复制。若要记录数量、长度等信息，应通过 modify 显式写入叶子。

自定义叶子合并规则必须以 Info{} 为左右单位元，才能直接接入/共享另一边。不能依据 sum==0 删除节点。`a.merge(a)` 默认会使叶子信息翻倍。

所有查询只读、不分配节点。Info 操作和 f 为 O(1) 时，单树查询与搜索为 O(log U)，M 根视图为 O(M log U)，U 为值域长度。点修改为 O(log U)。合并遍历两树同时存在的部分；独占节点复用，共享节点复制，存在共享时不能直接套用破坏性合并的整体均摊结论。

节点池只增长，没有回收。更新器和叶子规则不要递归修改同一模板实例下的树；搜索谓词也不应修改任何树。

## 无下推的边界与调研依据

当前 Info 是区间聚合结果，不承担等待传给孩子的更新。永久化的节点贡献（例如纯区间加）可以另做专题版本：它需要专门的查询和合并公式，不能任意混入此处的 `operator+` / `modify`。一般持久化 Lazy 在理论上可实现，复制后下推即可保护历史；这里主动不纳入骨架。

| 阅读的参考 | 本实现采用的经验 |
| --- | --- |
| [maspypy Dynamic Segtree](https://maspypy.github.io/library/ds/segtree/dynamic_segtree.hpp) | 路径复制；本版通过对象复制与自动 COW 保存历史 |
| [Nyaan Persistent Segtree](https://nyaannyaan.github.io/library/segment-tree/persistent-segment-tree.hpp) | 显式根、修改后用新孩子维护父信息 |
| [suisen Sparse Lazy Segtree](https://suisen-cp.github.io/cp-library-cpp/library/datastructure/segment_tree/sparse_lazy_segment_tree.hpp) | 明确空节点语义；本版统一使用 Info{}，不引入 Lazy |
| [suisen Persistent Lazy Segtree](https://suisen-cp.github.io/cp-library-cpp/library/datastructure/segment_tree/persistent_lazy_segment_tree.hpp) | 下推需要保护共享孩子；据此收敛为无下推核心 |
| [AtCoder Lazy Segtree](https://atcoder.github.io/ac-library/production/document_en/lazysegtree.html) | 明确区分汇总、更新映射与复合；本版仅保留汇总 |
| [OI Wiki 合并与分裂](https://oi-wiki.org/ds/seg-merge-split/) | 空子树接入、独立叶子规则、破坏性合并的适用范围 |
| [OI Wiki 标记永久化](https://oi-wiki.org/ds/seg/#标记永久化) | 不下推的特殊区间贡献应作为单独能力研究 |
