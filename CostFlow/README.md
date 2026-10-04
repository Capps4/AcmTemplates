# CostFlow

保留 CostFlow<Cap,Fee>、add、work 和三个模式：MAX_FLOW_MIN_COST 优先流量最大；MIN_COST_MAX_FLOW 优先费用最小、同费用选最大流；MIN_COST_MIN_FLOW 同费用选最小流。C++17 structured binding 和 if constexpr 简化优先队列/模式处理。顶点 0index，配对边编号 index^1；没有区间 API。

容量为非 bool、至多 64 位整数，费用为至多 64 位有符号整数。本实现明确用于整数费用流；不支持浮点费用/容量。容量非负，Fee 最小值不能取反、add 会拒绝。图可有自环/平行边；调用方不应直接修改公开的 edge/adj。work(s,t,limit) 返回本次额外的流量/费用，修改残量，单次流量最多 limit（默认 Cap 最大值）；s==t 或 limit==0 返回零。可重复调用或继续添加边。

初始负费用边使用队列 Bellman–Ford 建立可行势能，后续 Dijkstra 使用非负约化费用，每次调用重新建立势能以支持残量反向边。费用使用 pot[t]-pot[s]。从源可达的负费用环不在本算法契约内，会抛出 domain_error；不会静默死循环，也不做负环消去。不处理源不可达的纯费用循环。因此要用三种模式求整个网络最优，输入需没有负费用环；源不可达负环测试只证明不影响可达 s-t 求解。

距离/势能采用 GNU __int128，避免 64 位中间距离溢出。容量单次按 limit 裁剪以避免流量累加溢出，费用乘加用溢出检查、结果不适合 Fee 时抛出 overflow_error。出错增广尚未修改残量；此前已成功增广仍保留。返回类型仍 pair<Cap,Fee>；公开 pot/dist 元素改成 __int128，调用方需迁移相关输出。原版 float 模板实例若存在，需另选浮点专用算法，不应假设此整数实现能兼容。

5000 个小图（任意费用 DAG，以及所有环非负的循环图）穷举所有整数流分配，与三种模式/流量上限逐项比较，并检查最终流守恒/残量/费用；另测反向边重路由、分次求解、动态添边、零费用 tie、零流、自环、无效最小费用、64 位极值及 128 位乘积溢出、可达负环、二十万负费用链。GCC -O2 与 ASan/UBSan 验证。

最坏初始 Bellman–Ford O(VE)，后续每次增广 O((V+E)log V)，辅助空间 O(V+E)。公开结果较易继续求解，代价是宽整数、负边初始化扫描及校验；不保证比旧版所有场景快。七轮相同输入/校验和的旧新对照见 Performance.md/Results.json。类型大驼峰，方法/变量小驼峰，沿用三个旧枚举常量避免破坏调用方。

本轮以 Original 为基准：保留 Original primal-dual/三模式。负边初始势、宽整数及结果溢出检查是正确性修复；删除超大边数/负规模异常框架，保留实用流量上限与重复 work。

add 的合法容量/可取反费用由调用方保证；不再为负点数/负流量上限/巨大边数提供异常包装。结果费用溢出及可达负环检查保留。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check CostFlow`；性能用 `python3 template/Run.py bench CostFlow`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
