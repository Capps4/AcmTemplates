# PollardRho

[code.hpp](code.hpp)

`PollardRho(seed)` 使用 Brent 周期检测与批量 gcd，提供 `findFactor(n)` 和 `primeFactorize(n)`，依赖 MillerRabin。默认 seed 来自时钟，传固定 seed 可复现相同输入序列。

支持不超过 64 位的正整型，bool 除外；findFactor 要求 n≥2，素数返回自身，合数返回真因子。分解 1 返回空表，结果按质因子升序合并指数。

局部模数及 GNU 128 位精确模乘，不改变共享动态模数。寻找最小质因子 p 的典型工作量 O(sqrt(p))，随机重试没有固定最坏时间保证。
