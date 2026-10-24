# ModSqrt

`modSqrt(a, p)` 求 `x*x ≡ a (mod p)`，返回 `[0,p)` 中较小的根；无解返回 `-1`。p 必须是 `[2,INT_MAX]` 内的素数，调用方保证素性；a 接受完整 long long 范围，自动归一化。包括 p=2、a=0，不处理合数模数。

Tonelli–Shanks 使用 long long 精确乘法，无浮点误差。设试到首个二次非剩余共需 t 次探测，时间 O(t log p + log² p)，额外空间 O(1)；p≡3 mod 4 时直接幂运算 O(log p)。只读参数，无缓存或随机状态。

参考 [KACTL ModSqrt](https://github.com/kth-competitive-programming/kactl/blob/main/content/number-theory/ModSqrt.h)（CC0）的 Tonelli–Shanks 思路；本版显式处理无解、规范根及输入边界。测试使用小素数全根枚举及大素数平方构造。
