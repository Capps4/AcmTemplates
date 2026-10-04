# StronglyConnectedComponent

将 Tarjan 的递归 DFS 改为显式帧栈，避免长链触发系统栈上限。保留发现/low 数组、组件编号与凝聚图；类名 Scc，保留 SCC 别名兼容旧代码。结果完全拥有，不保存邻接表引用。

```cpp
Scc scc(adj);                 // 构建凝聚图 g
Scc labels(adj, false);      // 只需组件标号，跳过凝聚图分配
```

节点与组件均 0index，分别 `[0,n)`、`[0,cntBlock)`。对组件间边 x→y，有 bel[x]>bel[y]，组件编号为逆拓扑顺序，2-SAT 依赖此约定。g 保留平行边；未自动排序去重。buildCondensation=false 时 g 为空，但 bel/dfn/low/cntBlock 相同。时间 O(n+m)，额外空间 O(n)，凝聚图额外 O(n+m)；显式帧多用部分堆空间以消除调用栈限制。

测试遍历三节点以内所有有向图及两千随机图，用 Floyd 双向可达作为组件 oracle，检查凝聚图完整性、逆拓扑编号并与 TopSort 集成。另测二十万节点链/环与临时输入寿命。性能见 [Performance.md](Performance.md)，labels-only 对照体现跳过原版无条件构建凝聚图的收益，完整构建场景另列。

风格自审：Scc/Frame 大驼峰，现有 dfn/low/bel/cntBlock 保留。栈元素只在 enter 前使用，push 后不继续引用旧帧；顶点编号 int，合法边通过断言检查。

完整凝聚图构建此前约慢 12%~22%，显式栈带来额外状态；标签模式和 2-SAT 跳过未使用凝聚图后更快。以当前 Performance.md 为实际数值。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check StronglyConnectedComponent`；性能用 `python3 template/Run.py bench StronglyConnectedComponent`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
