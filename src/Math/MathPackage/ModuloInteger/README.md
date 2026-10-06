# ModuloInteger

[code.hpp](code.hpp)

`ModuloInteger<int/long long,P>`：P>0 固定模数，P=0 使用同类型共享动态模数；全局 Z 的模数为 998244353。值规范到 `[0,p)`，改变动态模数后须重建旧值。

int 模乘使用 long long；long long 模乘使用 long double 估商、32 位 limb 精确残差及校正，兼容 53 位 long double 和有符号 64 位模数。GNU 128 位仅用于宽输入规范化。

`setMod` 只用于 P=0，模数须正；负指数/除法使用素模逆元；负指数的底数及除法的除数须非零。支持 LLONG_MIN 指数及 constexpr；输入失败保留原值。基本运算 O(1)，快速幂/逆元 O(log p) 或 O(log |exp|)。
