# SuffixArray

[code.hpp](code.hpp)

`SuffixArray(s)` 倍增构建后缀数组，内部按 `countSort/equal/calcHeight` 分工。初排用 <、判等和 LCP 用 ==；字节按 unsigned char 排序，其他序列先排序压秩。

`sa[i]` 为第 i 小后缀起点，`rk[x]` 为 x 后缀排名，`h[i]` 为 sa[i] 与 sa[i-1] 的 LCP，h[0]=0。空输入允许，长度和下标须能放进 int。

时间 O(n log n)、空间 O(n)。
