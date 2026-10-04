# Kmp GNU++17

前缀函数 kmp(string_view)，返回每个位置的最长真前后缀长度。

接受非 NUL 结尾子串视图，参数按值，结果拥有存储；算法 O(n)，空间 O(n)，无额外字符串副本。

类型名大驼峰，函数和变量小驼峰；下标 0index，所有区间 [l,r)。函数名保留兼容。

## 验证

空串、所有长度 <=11 的二元串、随机子串、内嵌 NUL 和百万重复字符。

运行 `python3 ../Run.py check Kmp`（工作目录任意，需将脚本路径指向 template/Run.py）。测试覆盖 GCC -O2 和 Clang ASan/UBSan。

## 性能

完整串与子串；旧子串接口需 substr 创建副本，最终版通过视图传入。普通整串路径预期接近原版。

输入固定种子、预热一次、每项七轮中位数，旧/新结果校验和必须一致。实际数据见 Benchmark.csv 和 Results.json。仅代表此机器/编译器/工作负载，不推断所有输入下的速度。

## 交付与自我 Review

Final.hpp 为唯一实现来源；验证和交付直接使用头文件。已核对职责保持、API 前置条件、边界、复杂度及新增数据结构必要性。原 snippets 未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check Kmp`；性能用 `python3 template/Run.py bench Kmp`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
