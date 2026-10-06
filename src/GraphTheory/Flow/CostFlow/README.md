# CostFlow

[code.hpp](code.hpp)

`CostFlow<Cap,Fee>` 使用残量网络、势能 Dijkstra 和增广，支持源码列出的三种费用/流量优先模式。顶点从 0 开始；`add(x,y,cap,fee)` 加边，`work(s,t,lim)` 返回本次增加的流量与费用。重复调用继续使用残量网络。

容量须为至多 64 位整数，费用为至多 64 位有符号整数，反边费用须可表示。可达负环抛 `domain_error`，费用结果溢出抛 `overflow_error`；新增边和重复运行会重新初始化势能。存储 O(n+m)，一次 Dijkstra 为 O((n+m) log n)，总时间还取决于增广次数及初始势能计算。
