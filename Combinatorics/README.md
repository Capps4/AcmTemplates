# 组合数（GNU++17）

保持 Original factorial/inverse-factorial 的 jc/ijc/A/C 接口。素模且阶乘下标小于模数是前提。shared 返回同一份缓存引用，global comb 用 inline 引用，不复制整个表。

动态模改变会清空缓存，避免继续使用旧模数阶乘；init 无增长需求时直接返回。最多扩展到 modulus-1。节点和表大小符合 int 竞赛范围，不再做 INT_MAX limit 饱和包装。预处理 O(n)、一次逆元加线性回推、查询 O(1)。依赖：ModuloInteger。

本轮以 Original 为基准：Original factorial/inverse-factorial 缓存与 A/C 接口保留；inline shared 引用和动态模重置避免复制/旧模缓存。删除 INT_MAX limit 包装。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check Combinatorics`；性能用 `python3 template/Run.py bench Combinatorics`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
