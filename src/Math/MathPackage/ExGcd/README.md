# ExGcd

[code.hpp](code.hpp)

`exgcd(a,b,x,y)` 返回 gcd，通过引用输出 Bézout 系数 x/y，支持 constexpr。

a/b 为非负有符号整数，类型须容纳中间结果；a=b=0 时返回 gcd=0、x=1、y=0。欧几里得递归为 O(log(max(a,b)))。
