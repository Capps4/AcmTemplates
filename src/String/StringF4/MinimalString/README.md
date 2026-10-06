# MinimalString

[code.hpp](code.hpp)

`a | minimalString()` 返回字典序最小轮转的拥有型容器；左值复制、右值消费，string_view 物化为 string，空输入返回空容器。

使用 i/j/k 双候选算法，char 按 unsigned byte 比较。时间 O(n)，输出存储 O(n)，依赖 ListHelper。
