# Sieve / GNU++17

公开构造函数支持独立实例；`inline auto& siv` 引用共享实例。init(n) 在现有容量足够时直接返回，修正旧版重复初始化仍扩容的行为；mpf(x) 按 x+1 请求容量，保证边界下标可访问。线性筛乘积使用 uint64_t，避免旧 unsigned 乘法回绕。结构化绑定使约数生成更直接。

表覆盖 [0,size())，0index；构造/init 的 n 是排他上界。mpf(0)、mpf(1) 返回 0，mpf(x) 自动扩容。容量不超过 INT_MAX+1，实际值受内存限制。primes() 返回借用引用，init 扩容后引用到元素或迭代器可能失效。shared(n) 会满足后续容量请求。

primeFactorize(x) 接受 <=64 位整型（bool 除外）且 x>=1，1 返回空质因子列表。表内使用最小质因子，表外先除已知素数，再试除缺少的部分，边界使用 x/d 避免平方溢出。超大半素数应使用 PollardRho，本类表外试除最坏 O(sqrt(x))。allFactors 返回无序正约数；静态重载要求合法质因子分解且结果能用 T 表示。约数数目 D 时生成 O(D)。

测试：多种空/小表逐个比对试除 oracle 和朴素约数列表，验证扩容边界、重复 init、100000 内 9592 个素数；10000 个随机数、UINT64_MAX 分解及 128 个约数、小 unsigned char。GCC -O2、Clang ASan/UBSan 均通过。约束整型重载避免 Clang 的 allFactors 模板重载歧义。

性能：1000000 容量建表约 1.33x，100000 个表内数分解约 1.08x；七轮中位数与相同校验和见 Benchmark.csv。旧版本 benchmark 从小共享表复制成独立实例，避免共享表跨轮不断翻倍影响对比；新版直接构造独立实例。增长重筛 O(n)，几何扩容使总重筛成本 O(最终容量)，空间 O(n)。

自评：类型 Sieve 大驼峰，方法/变量 lowerCamelCase，容器 0index 与排他上界明确。修改限于筛、分解、约数及缓存生命周期。snippet 正文一致，源码与测试 SHA256 当前有效，原 VS Code 文件未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

本轮以 Original 为基准：Original 线性筛/试除与因子枚举保留；no-op init、mpf 边界、uint64 乘法和除法边界修复正确性。删除 MaxSize 防线；保留 scalar overload 的一行 SFINAE 消除 Clang 重载歧义。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check Sieve`；性能用 `python3 template/Run.py bench Sieve`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
