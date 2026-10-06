# NumberTheoreticTransform

[code.hpp](code.hpp)

实现为 `_ntt::Poly`，命名空间外 `using Poly = _ntt::Poly`。系数直接使用 Z；`root()` 从 `Z::getMod()` 编译期求最小原根，保存于 `Poly::G`。

模数须为编译期固定质数，默认 998244353 的原根为 3。任意原根均可用，长度 n 的单位根为 `G^((p-1)/n)`；二进制 NTT 要求 n 为 2 的幂且整除 p-1，默认最大长度 2^23。查根最坏试除 O(sqrt(p))，每个候选 O(k log p)，大模数可能超过 constexpr 计算预算。

提供 `dft/idft/mod/deriv/integr/inv/ln/exp/power/sqrt/prod`。inv 常数项非零、ln 为 1、exp 为 0；sqrt 只处理奇质数模下常数项 1 并取 +1 根。精度须小于模数、规模能放进 int，空精度允许。

合法长度卷积 O(n log n)、空间 O(n)，超过变换长度限制时回退朴素卷积。FFT 与 NTT 的全局 Poly 别名需自行区分。
