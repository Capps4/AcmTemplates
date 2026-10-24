# OfflineDynamicConnectivity

- `OfflineDynamicConnectivity(n,q)` 创建 `q` 个时刻，点编号 `[0,n)`。`add(l,r,x,y)` 添加无向边在半开区间 `[l,r)` 的生效记录，允许空区间、自环、重边和重复记录。
- `work(f)` 按 `0..q-1` 依次调用 `f(t, const RollbackDSU&)`；用 `same/size/find` 查询当前图。回调中的 DSU 仅在该次回调内有效，不保留引用、不修改调度器。
- 复用 `RollbackDSU`，不做路径压缩。增删事件应先转换为生效区间；重边事件需要按边 ID 或重数正确配对。`work` 每次重建 DSU，可重复调用，也可在调用后继续添加区间。
- `n >= 0`，`0 <= q <= INT_MAX/2`，`0 <= l <= r <= q`。`q=0` 不调用回调；`n=0` 允许空图时间线。
- `k` 条生效记录的时间为 `O(q + k log(q+1) log(n+1))`，另加回调成本；空间 `O(n+q+k log(q+1))`。递归仅用于时间树，深度不超过 31，与原图深度无关。

算法参考：[Nyaan Offline Dynamic Connectivity](https://nyaannyaan.github.io/library/graph/offline-dynamic-connectivity.hpp)（[CC0](https://github.com/NyaanNyaan/library/blob/master/LICENSE)）。这里用半开区间接口和仓内 DSU 独立实现，不引入其事件表或 HashMap。

验证：每时刻重新 BFS 对照连通性和块大小，穷举短区间，随机重边/空区间、重复 work、20K 时间线；性能用链边逐步生效与随机区间。OJ 排名待验证。
