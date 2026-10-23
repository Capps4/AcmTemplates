# ZFunction

[code.hpp](code.hpp)

`zFunction(string_view)` 返回 z[i]：s 与后缀 s[i..n) 的最长公共前缀，z[0]=0。下标从 0 开始，空输入返回空数组。

接受非 NUL 结尾子串视图，结果拥有存储；时间/空间 O(n)。
