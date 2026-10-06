# FastFourierTransform

[code.hpp](code.hpp)

实现位于 `_fft`，命名空间外定义 `Float=double`、`Poly=_fft::Polynomial<Float,std::complex>`。直接用 `Poly` 进行实数多项式卷积。

空多项式乘积为空；小卷积朴素计算，大卷积 FFT，时间 O(n log n)、空间 O(n)。浮点误差取决于长度及系数值域。

`Poly` 与 NTT 的全局别名同名，`Float` 也可能与其他模板同名；组合使用时自行区分别名。
