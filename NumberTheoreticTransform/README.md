# NTT（GNU++17）

保留 Original 的 NTT、Newton 迭代与多项式运算；类型名为 NttPolynomial<T,PrimitiveRoot=3>、别名 NttPoly，避免与 FFT 冲突。dft/idft、mod、deriv/integr、inv/ln/exp/power/sqrt/prod 和算术运算符保留。依赖固定模数 ModuloInteger。

T::getMod() 为固定奇质数，PrimitiveRoot 为对应原根。变换长度须为 2 的幂且整除 P-1；不支持的卷积长度走朴素乘法。inv 要求常数项非零；ln 要求常数项为 1；exp 常数项为 0；sqrt 仅处理常数项为 1 并取根 +1。积分与相关形式级数的有效精度不能越过模数。空变换/精度 0 安全。合法下标及规模能放入 int 由竞赛调用方保证，没有额外 MaxSize 饱和框架。

inline static 根表替代类外定义；原地复合运算和仅拷贝截断前缀避免大表复制，泛型递归替代 std::function。修复 Original 的空 integr 越界、power 提前 k%=P、prod 的 self 宏冲突，以及缺少变换根时的求逆回退。没有通用模平方根或任意模卷积框架。

独立朴素乘法/形式级数递推、变换互逆、小特征域、阈值、前导零、巨大指数、aliasing 和多数学模块联合测试见 Test.cpp。Original 自身含宏导致编译失败，性能基线仅在 LegacyBaseline.hpp 中更名 prod 未计时的递归参数，原快照未改动。

当前 GCC/ASan/UBSan 与七轮串行性能原始证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。部分全链收益来自更正确、更快的 ModuloInteger；同一新版 Z 的场景另行对照，不将其收益全部归给 NTT 内核。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check NumberTheoreticTransform`；性能用 `python3 template/Run.py bench NumberTheoreticTransform`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
