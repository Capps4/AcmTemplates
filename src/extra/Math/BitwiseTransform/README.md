# BitwiseTransform

`bitwiseTransform<BitwiseOp::Xor/And/Or>(a, inv=false)` 原地做 XOR Walsh–Hadamard / AND 上集和 / OR 子集和变换，inv=true 为逆变换。长度必须为 2 的幂；空数组作为空变换处理。

`bitwiseConvolution<Op>(a,b)` 要求等长，按值接收两个数组，返回 `c[k]=sum(a[i]*b[j], i op j=k)`。下标从 0 开始，不自动补长度；左值被复制，右值可 move，无借用或共享状态。

T 默认构造为零。AND/OR 需要加减，卷积还需要乘法；XOR 逆变换需要除以长度 n。域中 n 必须非零可逆；非整数类型只计算一次 1/n，整数类型逐项整除，要求逆变换的每项可被 n 整除且 n 可表示。内置整数的所有中间值必须可表示，不能依赖溢出；浮点存在舍入误差。推荐使用奇素数模数的 ModuloInteger，XOR 不支持特征 2 的域。

变换 O(n log n)、额外空间 O(1)；卷积 O(n log n)、数组空间 O(n)。if constexpr 只用于三种运算及整数/域归一化的实际差异。

参考：[Luzhiled FWHT](https://github.com/ei1333/library/blob/master/math/fft/fast-walsh-hadamard-transform.hpp) 的蝶形思路（Unlicense）。测试含整数/有限域朴素卷积、逆变换及大数组单位脉冲。
