# LagrangeInterpolation

`lagrangeInterpolation(y, x)` 返回唯一次数小于 n 的多项式在 x 处的值，采样约定 `y[i]=f(i)`、`0≤i<n`。空输入约定为零多项式。T 使用 core `ModuloInteger`，模数必须为素数，`n≤模数` 且 `n<INT_MAX`；x 传 T 类型，命中采样点直接返回。普通整数、浮点及重复采样点不在支持域内。

复用 `Comb<T>` 的阶乘逆元，前缀积和滚动后缀积计算每个基函数。单次求值 O(n) 时间、O(n) 临时空间；首次扩充共享组合数缓存另需 O(n+log p) 时间和 O(n) 缓存空间。运行时模数切换沿用 Comb 的自动失效规则；切换后须重新构造 y、x。输入只读，无额外持久状态。

示例：`std::vector<Z> y{1, 4, 9}; auto v = lagrangeInterpolation(y, Z(10));` 得到 121。

参考 [OI Wiki 插值](https://oi-wiki.org/math/numerical/interp/) 的连续点拉格朗日公式；不重复实现 NTT 或一般点插值。测试由独立 Horner 求值生成多项式样本，并覆盖小域全枚举和动态模缓存切换。
