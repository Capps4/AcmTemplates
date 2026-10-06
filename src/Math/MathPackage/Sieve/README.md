# Sieve

[code.hpp](code.hpp)

`Sieve` 使用线性筛，`init(n)` 的 n 为排他上界，表覆盖 `[0,size())`；`mpf(x)` 自动扩容，0/1 返回 0。`primes()` 返回借用引用，扩容后旧元素引用和迭代器失效；全局 siv 引用共享实例。

`primeFactorize(x)` 接受不超过 64 位的正整型，1 返回空表；表外先除已知素数再试除，最坏 O(sqrt(x))。大半素数使用 PollardRho。`allFactors` 返回无序正约数，结果须可表示。

筛构建 O(n)、空间 O(n)，D 个约数的生成 O(D)；容量不超过 INT_MAX+1，实际受内存限制。
