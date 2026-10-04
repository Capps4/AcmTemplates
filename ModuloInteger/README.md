# ModuloInteger GNU++17

保留 ModuloInteger<T,P>、getMod/setMod、val、power/inv 及运算/I/O API。T 为 int 或 long long，P>0 是静态模数，P==0 是按 T 共享的动态模数。动态模数必须正，存活值使用期间不能切换；setMod 仅对 P==0 可用。inv、除法和负指数仍要求素数模数及非零被除数/基底。

if constexpr 选择固定/动态模数与有符号/无符号构造，inline static 模数省去类外特化定义。完整 constexpr 运算链可用于常量表达式。加法用无符号中间值避免接近类型上限时溢出；long long 乘法用 GNU __int128 精确取模，替代原版存在有符号溢出的浮点商估算。power 支持 LLONG_MIN 负指数而不做有符号取负。输入失败保留原值。

0index/[l,r) 不适用于标量算术；类型大驼峰，函数/变量小驼峰。构造接受标准整型和 GNU 128 位整型，归一化到 [0,mod)。默认 Z 和 P 保持原值。

Test.cpp 用 __int128 参考实现验证十种模数、20 万对输入、LLONG_MIN/ULLONG_MAX、常量表达式、素数逆元和输入失败，GCC -O2 与 Clang ASan/UBSan。基准比较固定/动态 int，以及旧版所有中间值均可表示的小模数 long long；不拿旧版未定义行为的速度作为正确实现的门槛。

自我 Review：保持原来的 Fermat 逆元职责，不引入大整数、任意模逆元或新型乘法后端；精确 128 位取模可能增加 long long 运算常数，实际取舍与数据保存在 Benchmark.csv/Results.json。不会将正确性修复宣称为加速。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check ModuloInteger`；性能用 `python3 template/Run.py bench ModuloInteger`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
