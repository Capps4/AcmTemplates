# MillerRabin / GNU++17

删除运行期 vector 与 ModuloInteger 动态模数依赖；见证常量无需堆分配，判定对象无状态且 operator() const constexpr。if constexpr 选择 32 位 uint64_t 乘法或 64 位 GNU unsigned __int128 乘法。inline constexpr isPrime 可直接用于 static_assert，调用不会污染公共动态模数。

支持 <=64 位有符号/无符号整型（bool 除外）。负数、0、1 返回 false。32 位见证 2/7/61，64 位见证 2/325/9375/28178/450775/9780504/1795265022，覆盖完整 uint64_t 范围。依据 Forišek/Jančina 论文 [Theorem 3](https://ceur-ws.org/Vol-1326/020-Forisek.pdf)，见证基数必须先模 n；余数 0 的见证跳过。该结论的范围是 n<2^64，不能延伸到更宽整数。

测试：0..1000000 全量比对 Eratosthenes 独立筛；多个强伪素数、signed/unsigned 边界、最大附近 64 位素数、编译期判定。2000 个随机完整 uint64 输入以逐次加法模乘实现独立算术 oracle，使用同一已验证见证集。验证已有动态模数和对象不受调用影响。GCC -O2 与 Clang ASan/UBSan 均通过。

性能：200000 个混合整数，32 位约 1.36x，64 位小整数约 1.46x；详细七轮中位数见 Benchmark.csv。64 位旧版测试只在 <=1000000 上比较，避免原 ModuloInteger 的 signed 乘法溢出；没有用 UB 基准证明大整数性能。复杂度 O(log n) 个宽整数模乘，O(1) 空间。

自评：类型名大驼峰，方法/变量 lowerCamelCase，素性判定无索引或区间接口。GNU++17 的 __int128 是必要扩展，不宣称它来自 ISO C++17。没有引入更大的哈希见证表。源/snippet 一致，测试与依赖证据保存于 Results.json，原 VS Code 文件未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check MillerRabin`；性能用 `python3 template/Run.py bench MillerRabin`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
