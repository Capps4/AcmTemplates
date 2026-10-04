# CountingSortOrder GNU++17

countingSortOrder(a, key)，返回稳定升序的原始下标。

默认 IdentityKey 保留具体类型；比较和遍历使用 const 引用；支持 move-only key，处理空输入。O(n+maxKey)，需要非负 int 键、纯投影和元素数 <=INT_MAX。大范围稀疏键应改用比较排序。

类型名大驼峰，函数和变量小驼峰；下标 0index，所有区间 [l,r)。函数名保留兼容。

## 验证

标准库 stable_sort 对照，空数组、重复/稀疏键、256 字节记录和 move-only 投影。

运行 `python3 ../Run.py check CountingSortOrder`（工作目录任意，需将脚本路径指向 template/Run.py）。测试覆盖 GCC -O2 和 Clang ASan/UBSan。

## 性能

整数两种键范围、256 字节记录。不会为避免重复 key 调用额外分配完整键数组。

输入固定种子、预热一次、每项七轮中位数，旧/新结果校验和必须一致。实际数据见 Benchmark.csv 和 Results.json。仅代表此机器/编译器/工作负载，不推断所有输入下的速度。

## 交付与自我 Review

Final.hpp 为唯一实现来源；验证和交付直接使用头文件。已核对职责保持、API 前置条件、边界、复杂度及新增数据结构必要性。原 snippets 未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check CountingSortOrder`；性能用 `python3 template/Run.py bench CountingSortOrder`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
