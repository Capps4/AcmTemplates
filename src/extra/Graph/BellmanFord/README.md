# BellmanFord

- `BellmanFord<T>(n)`，点编号 `[0,n)`；`T` 为不超过 64 位的有符号整数。`add(x,y,w)` 添加有向边并返回原边 ID，支持重边、自环及最小负整数。
- `work(s)` 从头计算，返回 `state / dis / pre / cycle`。状态为 `Unreachable / Finite / NegativeInfinity`；仅 `Finite` 的 `dis` 有意义，类型为 `__int128_t`。`pre[v]` 是前驱边 ID，有限点可沿它恢复最短路，源点及不可达点为 `-1`。
- `NegativeInfinity` 标记源可达负环及其全部后继。`cycle` 是其中一个负环按行走顺序排列的原边 ID；没有可达负环时为空。负无穷点的 `pre` 仅是松弛记录，不能按最短路使用。
- `n >= 0`、边数不超过 `INT_MAX`；调用 `work` 必须存在合法源点。最多 `n*m` 次松弛，64 位边权的中间值可由 128 位整数容纳，不截断成有限的负无穷哨兵。
- 时间 `O(nm+n+m)`，额外空间 `O(n+m)`；无图递归。结果拥有数据，后续 `add/work` 不影响已返回结果。

算法参考：[cp-algorithms Bellman–Ford](https://cp-algorithms.com/graph/bellman_ford.html)（资料为 [CC BY-SA 4.0](https://github.com/cp-algorithms/cp-algorithms/blob/master/LICENSE)）。这里按三态距离、原边恢复和宽整数契约独立实现。

验证：三点权值穷举、Floyd 三态 oracle、随机重边/自环、负环后继与不连通负环、64 位极值、20K 链；性能用逆序负权链及带势函数的稀疏图，记录本机结果，OJ 排名待验证。
