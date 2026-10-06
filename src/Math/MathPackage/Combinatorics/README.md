# Combinatorics

[code.hpp](code.hpp)

`Comb<T>` 提供 `jc/ijc/A/C`，分别查询阶乘、逆阶乘、排列数、组合数；全局 `comb` 使用 `Z`。`shared()` 返回共享缓存引用，`init(n)` 扩展预处理。

要求素模，阶乘下标小于模数；动态模数变化会重建缓存。首次预处理 O(n) 次域运算及一次求逆，查询 O(1)，空间 O(n)。
