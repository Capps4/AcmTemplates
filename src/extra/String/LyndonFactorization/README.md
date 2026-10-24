# LyndonFactorization

`lyndonFactorization(s)` 返回含首尾的下标边界 b，每段为 `s[b[i],b[i+1])`。这些 Lyndon 串按字典序不增，拼接为原串；空串返回 `{0}`。字典序按 unsigned char 比较，支持 NUL 和高位字节。长度须小于 INT_MAX。

Duval 算法 O(n) 时间；除输出外 O(1) 空间，输出最多 n+1 个边界。借用 string_view 仅限调用期间，不保存视图、不复制子串，无共享状态。

参考 [cp-algorithms Lyndon factorization](https://cp-algorithms.com/string/lyndon_factorization.html) 的 Duval 算法。测试独立枚举所有切分，检查每段严格小于其全部真后缀及因子顺序，另覆盖随机字节和长重复串。
