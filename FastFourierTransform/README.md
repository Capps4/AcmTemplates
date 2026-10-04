# FFT / GNU++17

保留 Polynomial<T,Complex>、Float、Poly 及卷积 operator* 接口。inline static 保存单位根与位反转缓存，删除类外静态成员定义。只存一份半圈单位根，if constexpr 在逆变换中取共轭；原来的两份完整单位根表由 2n 个复数缩为 n/2 个。缓存含位反转表的有效数据约由 36n 字节减到 12n 字节（double/complex<double>/int 的本机布局），不包括卷积的临时数组。

T 要求浮点类型；默认 double，也测试了 long double。小输入保留原来 min(sizeA,sizeB)<128 的朴素卷积阈值，大输入补到不小于结果长度的 2 次幂。删除内部 std::__lg 依赖以及 2*n-1 的 signed 溢出。非空卷积输出长度为 a.size()+b.size()-1，无隐式整数舍入；需要整数答案时，调用者必须在误差可接受的范围内自行舍入。浮点系数可能有舍入误差，不保证任意整数精确卷积。

容器 0index；类型 Polynomial/Poly 大驼峰，方法与内部变量 lowerCamelCase。普通朴素输出长度不超过 INT_MAX，大输入 FFT 输出长度不超过 2^30，实际可用规模受内存限制。缓存最后一个变换长度，切换长度会重建；共享缓存不支持并发调用。与 NTT 保留相同 Polynomial/Poly 名称，属于两种替代模板，同一翻译单元使用两者时需自行放到不同 namespace，这项命名兼容性仍列入最终集成检查。

测试：double/long double 随机正负分数卷积逐项对照 long double 朴素卷积；空输入、单项、127/128/129 阈值、2 次幂附近长度、反复切换缓存。百万规模之外的 10000 个 1 验证三角形卷积，脉冲验证移位结果。GCC -O2 与 Clang ASan/UBSan 均通过。

性能：4096/16384 同长输入热缓存约 1.04x/1.03x，4096 与 8192 交替重建缓存约 1.25x；微小输入不足以精确判断速度。完整七轮中位数在 Benchmark.csv，浮点结果舍入为整数校验和，与旧版一致。主要收益是缓存体积、初始化和代码简化，不宣称改变 O(n log n) 核心复杂度。全量朴素误差检查在 Test.cpp，基准校验和不替代精度测试。

自评：算法方向、默认精度和 threshold 保留；没有把 constexpr 加到不能在 C++17 常量求值的 vector 算法上。源码/snippet 匹配，所有测试与基准头文件 SHA256 记录于 Results.json。原 VS Code 文件未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

集成命名更新：Polynomial→FftPolynomial、Poly→FftPoly、Float→FftReal。不再导出冲突的旧全局别名；调用方按上述名称迁移，可在自己的局部作用域另设短别名。当前源码与性能数据已按新名字重测，本文较早示例中的旧名字按此替换。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check FastFourierTransform`；性能用 `python3 template/Run.py bench FastFourierTransform`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
