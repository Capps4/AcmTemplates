# RandomNumber

[code.hpp](code.hpp)

全局 `rng` 为共享 `mt19937_64` 引擎，默认使用稳态时钟种子。`rng.seed(seed)` 可重放确定序列，搭配标准 distribution 使用。

inline 变量保证跨翻译单元共享同一实例；并发修改须外部同步。默认种子不保证进程之间绝不重复。
