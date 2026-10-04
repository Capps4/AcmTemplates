# TopSort

保留 Kahn 算法，用结果 vector 同时作 FIFO 队列，避免单独的待处理栈与一次结果写入。多个合法序列间的选择由旧版 LIFO 改为 FIFO；需要特定字典序时应使用对应的优先队列算法。遍历边改为 const 引用，支持 pair<int,不可复制权重>；预留 result 空间，入度采用 size_t，避免大量平行边使 int 入度溢出。C++17 inline constexpr topSort 可多翻译单元共用。

`topSort(adj)` 支持 int 邻居及 pair<int,Weight> 边。节点 0index，合法节点 `[0,n)`，复杂度 O(n+m)、额外空间 O(n)。结果大小等于 n 当且仅当无环；有环时返回能移除的前缀，不能当作完整拓扑序。空图返回空序列。

测试遍历 n<=3 全部有向图，以 Floyd 判断是否有环，同时独立模拟入度删除验证结果；包含随机多重边、自环、unique_ptr 权重及二十万节点链。性能见 [Performance.md](Performance.md)，基准含全长链使拓扑序唯一，校验和可直接比较。

风格自审：TopSort 大驼峰，小驼峰局部/函数；无递归、无额外图副本，保留 endPoint 私有重载。接口不处理权重值，借用输入仅在调用期间。

当前链与 DAG 场景约慢 5%/8%，交换的是单个队列存储、支持不可复制权重以及 size_t 入度；算法仍为线性时间。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check TopSort`；性能用 `python3 template/Run.py bench TopSort`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
