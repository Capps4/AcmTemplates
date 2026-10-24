# LinearRecurrence

`linearRecurrence(a, c, k)` 返回从 0 开始的第 k 项，递推约定 `a[i]=sum(c[j]*a[i-j-1], j=0..d-1)`，d=c.size()。a 至少提供前 d 项；额外已知项必须满足同一递推，k 在已知范围内时直接返回。空递推在已知范围之后返回零；a、c 都空时恒为零。k 使用完整 uint64_t 范围，不做 k+1。

T 需要默认零、`T(1)`、加法及乘法，构成交换环；所有中间值需可表示。使用特征多项式取模的二进制幂，时间 O(d² log(k+1))、额外空间 O(d)。d≤INT_MAX/2。

`berlekampMassey(a)` 返回能拟合已知序列的最短递推系数，顺序与上面一致。T 必须为支持精确零比较、加减乘除的域，不能用普通整数或有舍入误差的浮点；推荐固定素数模数的 ModuloInteger。长度≤INT_MAX/2，空序列及全零序列返回空递推。时间 O(n²)、空间 O(n)。

有限前缀的拟合本身不保证未来：已知序列确实具有阶数≤d 的齐次线性递推时，至少提供前 2d 项才能恢复可用于外推的递推。两个函数只读输入，无共享状态。

参考：[KACTL LinearRecurrence](https://github.com/kth-competitive-programming/kactl/blob/main/content/numerical/LinearRecurrence.h)、[BerlekampMassey](https://github.com/kth-competitive-programming/kactl/blob/main/content/numerical/BerlekampMassey.h)（CC0）的多项式约减和最短递推思路。本版使用泛型系数和 uint64_t 下标。测试含小域穷举、独立顺推、随机恢复及极大下标。
