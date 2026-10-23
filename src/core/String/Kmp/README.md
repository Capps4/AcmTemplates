# Kmp

[code.hpp](code.hpp)

`kmp(string_view)` 返回前缀函数：每个位置结尾的最长真前后缀长度。下标从 0 开始，空串返回空数组。

输入可以是非 NUL 结尾子视图，结果拥有存储；时间与空间 O(n)。
