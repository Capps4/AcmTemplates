# EdgeBiconnectedComponent

递归 DFS 改为显式帧栈。类名 EdgeBc，保留 EdgeBC 别名；结果完全拥有，不保存原邻接表引用。帧只保存顶点、待跳过的父边目标和游标，父节点由上层帧取得；只跳过一条父边，保留平行边的回边效果。

```cpp
EdgeBc blocks(adj);
// blocks.bel[v]：边双组件；blocks.bridges：桥端点；blocks.g：桥树森林
```

输入是合法无向多重图，每条物理边在两端各存一项，自环存两项。节点/组件均 0index，对应 `[0,n)`、`[0,cntBlock)`。componentNum 为原图连通分量数，bridges 是 DFS 父→子的桥端点；原 cutDeg 字段此前始终为 0，现在表示该顶点关联的桥数。g 通过桥表生成，避免重扫非桥边；邻接顺序不承诺与旧版相同。空输入、孤点、平行边、自环均支持。

时间 O(n+m)，输出加 scratch 为 O(n+m)，其中桥树及桥表 O(n)；显式帧使用堆内存以消除递归深度上限。dfn/low 保持标准父边跳过的 low-link 语义。

测试遍历 n<=5 全部简单无向图，逐物理边删除后用独立 BFS 判断桥，再同时删除所有桥检查组件与桥度；另测三千随机含重边/自环图、二十万链/环、临时输入寿命。性能见 [Performance.md](Performance.md)，原版只运行安全深度的链、星形和多重图。新增桥表/桥度以及显式状态不是所有场景都加速，当前链场景有回退。

风格自审：EdgeBc/Frame 大驼峰，方法与变量小驼峰；保留数学字段和旧别名，明确 0index、边与组件契约。没有新增边 ID 型输入或图容器封装，帧扩容后不继续使用旧引用。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check EdgeBiconnectedComponent`；性能用 `python3 template/Run.py bench EdgeBiconnectedComponent`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
