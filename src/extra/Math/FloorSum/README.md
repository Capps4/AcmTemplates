# FloorSum

`floorSum(n, m, a, b)` 返回 `sum(floor((a*i+b)/m), i=0..n-1)`，输入是 uint64_t，m>0，n=0 返回零。不接受负数。

返回 unsigned __int128；乘法使用宽整数，结果超过 128 位时抛 `std::overflow_error`，不会静默截断。完整 uint64_t 参数的数学结果可能达到约 191 位；n≤2^32 时任意 uint64_t 的 a、b 均可表示。无共享状态或重置要求。

时间 O(log m)，额外空间 O(1)。

参考：[AtCoder Library internal_math](https://github.com/atcoder/ac-library/blob/master/atcoder/internal_math.hpp) 的欧几里得递降思路（CC0）；本版扩展宽中间值和结果溢出检查。测试含独立逐项求和、小范围穷举及 64/128 位边界。
