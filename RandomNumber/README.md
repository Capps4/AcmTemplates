# RandomNumber

原版只有一个 mt19937_64 引擎，运行算法无须重写。使用 C++17 inline 全局变量，在多翻译单元包含时保持一个引擎实例，避免重复定义；保留 rng 名称与稳态时钟初始种子。

可通过 `rng.seed(seed)` 重放确定序列，标准 distribution 仍可直接使用。默认种子不保证不同进程绝不重复；共享引擎需调用方同步，不能并发无锁修改。

TestExtra.cpp 是独立编译链接的真实翻译单元，检查地址相等、交替取数与独立参考引擎一致，另测重播及分布范围。性能见 [Performance.md](Performance.md)：仅修改链接语义，不承诺取数加速。

风格自审：没有添加不必要包装类。rng 小驼峰；没有下标或区间接口。最终版只需 chrono/cstdint/random 标准头，GNU++17 与 Clang 的 C++17 均能编译。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check RandomNumber`；性能用 `python3 template/Run.py bench RandomNumber`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
