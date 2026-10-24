# DiscreteLog

`discreteLog(a, b, m)` 返回满足 `a^x ≡ b (mod m)` 的最小非负 long long 指数；无解返回 `-1`。`1≤m≤INT_MAX`，a、b 接受完整 long long 范围并归一化。支持 a 与 m 非互质；约定 `a^0=1`（包括 a=0），因此 m=1 恒返回 0。

先约去公因子，再用扩展 BSGS；中间乘积使用 long long，不支持完整 64 位模数。哈希表平均时间 O(√m + log m)，空间 O(√m)，最坏哈希碰撞不保证此时间界。每次调用独立，无持久缓存。内存规模由原始模数的平方根上界控制。

参考 [cp-algorithms Discrete Logarithm](https://cp-algorithms.com/algebra/discrete-log.html) 的约因子和分块思想，按本仓接口实现。测试对小模数完整枚举幂状态直到首次重复，核对无解和最小性；另覆盖大模数及负输入。
