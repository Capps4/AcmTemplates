# MaxFlow

沿用 Original 的递归 Dinic：bfs 建层图，dfs 按当前弧增广，work 累计流量。删除 Frame、显式栈 stk 和 blockingFlow；BFS 的可复用 vector 队列继续保留。

```cpp
Flow<long long> flow(3);
flow.add(0, 1, 7);
flow.add(1, 2, 7);
auto first = flow.work(0, 2, 3); // 3
auto rest = flow.work(0, 2);     // 4
```

DFS 保留 Original 的 u/v/i/j/f/res/out 名称和递归、当前弧、正反边更新结构。res 从 0 累计已发送流量，f 保存剩余流量；这样避免浮点容量下 Original 的 Inf-(Inf-small) 返回 0 的问题。work 保留可选流量上限，避免总流量超出返回类型；可以重复调用继续增广。source==sink 或 limit==0 返回 0。

容量 T 为非 bool 算术类型，容量非负，浮点容量有限。每对正反边初始容量之和须可由 T 表示。节点和边 ID 从 0 开始，节点范围 [0,n)，add 返回正向偶数边 ID，反向为 id^1。支持平行边、自环、初始反向容量和 newNode。公开 g/adj/n，但调用方须保持边对、节点范围和非负残量。

getReach 用字符 s/t 表示残余可达集合，getCuts 返回分界端点对，可能包含反向边或重复端点。只有汇点不再可达时，该分界才是最小割；达到限流后可能仍需继续增广。原图割容量需要使用调用方保存的原始容量计算。

标准 Dinic 最坏时间 O(n²m)，存储 O(n+m)，递归栈深度随增广路径长度增长。采用 Original 的递归写法，不再承诺二十万节点长链的栈安全。

三千随机小图枚举最小割核对；另测反向容量、自环、平行边、限流、重复调用、新增节点、uint64 最大容量、浮点分数和两千节点链。GNU++17 和 ASan/UBSan 结果及七轮计时见 Results.json、Performance.md。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check MaxFlow`；性能用 `python3 template/Run.py bench MaxFlow`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
