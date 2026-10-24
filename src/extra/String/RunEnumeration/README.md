# RunEnumeration

`runEnumeration(s)` 返回按 `(p,l,r)` 排序且不重复的三元组，区间为 `[l,r)`，p 是该子串的最小正周期，`r-l≥2p`，向左右扩展一个字符均不能保持周期 p。空串和单字符返回空。支持任意字节，长度≤INT_MAX/2，结果下标使用 int。

分治枚举跨中点的重复串，复用 core `zFunction`。直接检查原串两端的极大性，以 `(l,r)` 精确编码为哈希键并保留最小周期；不使用概率字符串哈希。平均时间 O(n log n)、额外空间 O(n)，递归深度 O(log n)；unordered_map 的最坏碰撞不保证上述时间界。视图只在调用期间借用，不保留输入或共享状态。

参考 [Nyaan Run Enumeration](https://nyaannyaan.github.io/library/string/run-enumerate.hpp.html)（[CC0](https://github.com/NyaanNyaan/library/blob/master/LICENSE)）的分治与 Z 扩展思路；本版直接验证全局极大性并按区间合并周期。测试独立枚举全部区间、全部周期，与完整输出比较；另覆盖反转、字节重命名、长同字符和周期串。
