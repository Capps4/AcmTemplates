# PollardRho / GNU++17

采用 Brent 周期检测与 128 项批量 gcd；std::gcd 替代自写递归 gcd。局部模数、unsigned __int128 精确 64 位乘法和溢出安全模加，消除公共动态模数污染。if constexpr 为 <=32 位选择 uint64_t 乘法。构造函数可传 seed，便于复现；默认种子仍来自 steady_clock。primeFactorize(1) 返回空列表。

依赖新版 MillerRabin（先插入 snippet）；Final.hpp 自动包含对应头文件。支持 <=64 位正整型（bool 除外）。findFactor(n) 要求 n>=2，返回 n 当且仅当为素数，否则返回 [2,n) 的真因子。批次 gcd==n 时逐项回退，失败或尝试过长则重新随机选参数。分解列表按质因子升序合并指数。旧版本会在乘积 0 前停止更新，因此在精确模运算下不能简单断言旧 gcd 会返回 n；主要修正是算术范围、共享模数与可复现性。

算法是随机算法，确定种子可复现同一输入序列，运行时间仍依赖输入与随机轨迹。单次找最小质因子 p 的典型工作量 O(sqrt(p))，不能保证严格最坏运行时间。没有固定重试次数或把未完成分解当成素数。

测试：4 个 seed 下对 2..9999 完整比对试除分解；检查返回真因子契约。验证大半素数、31 位素数平方、UINT64_MAX、2^63、3^40、接近 UINT64_MAX 的素数与 32 位上界，公共动态模数不变。GCC -O2 与 Clang ASan/UBSan 均通过。

性能：5000 个混合小数约 2.00x，2000 个小合数约 1.23x；七轮中位数见 Benchmark.csv。旧版随机数器不能外部设 seed，旧路径跨轮存在随机变化，新版每轮固定 seed；因此这些数字用于本机回归检查，不能保证任意输入同等加速。旧/新均使用 int 且数 <=1000000，旧乘法在该范围内合法；不比较原版溢出的 64 位大整数。

自评：类型 PollardRho 大驼峰，方法/变量 lowerCamelCase，内部 vector 从 0 索引；[2,n) 因子范围明确。结果只含已判定为素数的因子，退化 gcd 不被误当作成功。源/snippet 一致，Results.json 记录 MillerRabin 与原版 benchmark 依赖文件 SHA256，原 VS Code 文件未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check PollardRho`；性能用 `python3 template/Run.py bench PollardRho`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
