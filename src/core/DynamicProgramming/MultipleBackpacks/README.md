# MultipleBackpacks

[code.hpp](code.hpp)

`multiBag(VecGood{...},capacity)` 处理 `{value,weight,count}` 多重背包，返回每个容量下总重量至多该容量的最大价值。

容量、重量、数量非负，capacity<INT_MAX，累计价值须能放进 long long。零数量/非正价值/超容量物品跳过，正价值零重量物品直接加到所有容量。

有效物品使用余数链单调队列，时间 O(物品种数×capacity)、额外空间 O(capacity)。
