# VertexBiconnectedComponent

递归点双 DFS 改为显式帧栈，保留圆方森林 csqt，新增 isCut。类名 VertexBc，保留 VertexBC 别名；结果拥有，不保存原图引用。预留圆方节点和工作栈，避免链图上频繁重分配。

```cpp
VertexBc blocks(adj);
// csqt[0..n)：原顶点；csqt[n..)：点双块，双向连接所属顶点
// isCut[v]：是否割点
```

输入为合法无向多重图，每条边两端各存一项；自环存两项，但不单独产生点双块，因为这里的结果描述顶点连通与分离关系。孤点仍保留为孤立圆点。节点/圆方下标均 0index，原顶点 `[0,n)`、块 `[n,csqt.size())`；所有编号须可由 int 表示。componentNum 为原图连通分量数。

只跳过一条父边，采用标准 `low[child]>=dfn[parent]` 产生块。旧版不跳过父边，因此 low 值可能比新版低；使用旧 `low==dfn[parent]` 判据的调用方需更新。块成员和圆方森林的含义保持，顺序不作为接口承诺。isCut 等价于原顶点关联至少两个块，适用于 DFS 根及非根。

时间 O(n+m)、额外空间 O(n+m)。显式帧消除长链递归上限；当前星形/多重图基准有回退，结果还新增割点标记，性能详见 [Performance.md](Performance.md)。

测试遍历 n<=5 全部简单图，并用独立顶点删除 BFS 判断割点与任意点对是否共享块；枚举最大集合，与输出块完整对照。另含随机平行边/自环、圆方森林边数检查、二十万链/环。避免用另一份 Tarjan 作正确性 oracle。

风格自审：VertexBc/Frame 大驼峰，保留 csqt/dfn/low 数学字段和别名；0index/半开编号界限明确。工作帧省去单独父节点/布尔数组，父边只跳过一次，pop 后不继续引用旧帧。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check VertexBiconnectedComponent`；性能用 `python3 template/Run.py bench VertexBiconnectedComponent`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
