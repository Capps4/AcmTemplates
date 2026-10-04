# GNU++17 简化与编译期优化

当前实现 364 行，公开 API、16B int SBT 节点和原生 vector<bool> 保持不变。

- `[]` 与 `()` 使用 `decltype(auto)`，保留普通引用及标准库代理的准确类型；这是 GNU++17 可用的语法简化，不宣称带来速度提升。
- 插入的 Mode 原本已用 `if constexpr`。这次进一步用 `if constexpr`、`std::is_trivial_v`、`std::is_trivially_move_assignable_v` 在编译期选择 payload 清理策略。
- 同时满足两个类型条件的 Value 在 erase/clear 时省去不可访问旧值的重置；所有重新插入路径仍显式赋值，缺失 [] 仍初始化 Value{}。
- 非平凡默认构造或赋值、资源持有类型保留原来的重置，回归测试包括默认构造计数、模板赋值副作用，以及 weak_ptr 观察即时释放。原有 const bool 代理、移动、复制、排名和差分检查继续保留。
- 测试脚本默认用 GNU++17 跑性能，同时验证 GCC GNU++17、GCC 严格 C++17、Clang 严格 C++17 及 ASan/UBSan。

## 相同进程内的前后对照

GNU GCC 15.2，`-std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic`，macOS arm64。每个容器预先插入 N=1,000,000 项；Off 的候选 U=N，全部活跃。所有值均为非零（bool 全 true，int 为 key+1）。每组合预热一次，再交错 AB/BA 测量 5 轮。

clear 表为**一次清空百万元素的总时间，单位 ms**；离线还需要清计数/存在数组，SBT 对满足条件的 Value 只重置三个结构字段。

| 容器 | Value | 优化前中位 ms | 优化后中位 ms |
| --- | --- | ---: | ---: |
| TreeMap | bool | 1.576 | 低于本次计时分辨率 |
| TreeMap | int | 0.238 | 低于本次计时分辨率 |
| TreeMapOff | bool | 1.609 | 0.020 |
| TreeMapOff | int | 0.266 | 0.019 |

SBT clear 读数为 0 不代表绝对零耗时，也不据此计算倍率；它已经消除了线性 payload 循环。相同条件下完整随机删除 N 个键的结果如下，单位 ns/次删除：

| 容器 | Value | 优化前中位 | 优化后中位 |
| --- | --- | ---: | ---: |
| TreeMap | bool | 196.262 | 203.932 |
| TreeMap | int | 226.589 | 210.927 |
| TreeMapOff | bool | 100.008 | 98.268 |
| TreeMapOff | int | 99.189 | 98.790 |

N 恰为 100 万，因此这张删除表的 ns/次与整轮总 ms 数值相同。收益主要集中在 clear；SBT bool 删除中位数略慢，前后范围重叠，不能宣称所有操作都加速。其他删除项改善较小，保留逐轮数据以展示波动。

计时前后有 compiler memory barrier 和 atomic_signal_fence，避免旧版写回因随后的逻辑清空而被删掉。容器跨计时存活；初始化、完整初态检查和末态验证均在计时外。每次运行还验证删除返回值、空状态及原非零槽位重新 [] 插入得到默认零。16 次预热和 80 次正式运行全部通过校验。

## 复现

[原始数据](resetResults.csv)、[中位数与范围](summary.csv)、[运行日志](resetResults.log)、[修改补丁](optimization.patch)、[测试源码](benchReset.cpp)。正式五方 GNU++17 评测见 [PERFORMANCE.md](../../PERFORMANCE.md)。

下面命令针对本报告记录的 364 行历史版本，需先恢复 [源码指纹](sourceSha256.txt) 对应的 after.hpp。当前正式头文件已有排版、命名空间和排序调整，不能直接代入旧补丁。当前版本的对照复现请使用 [构建优化报告](../buildOptimization/README.md)。

```bash
mkdir -p /tmp/treeMapGnu17Repro
cp treeMap.hpp /tmp/treeMapGnu17Repro/after.hpp
cp treeMap.hpp /tmp/treeMapGnu17Repro/before.hpp
patch -R /tmp/treeMapGnu17Repro/before.hpp < final_results/gnu17/optimization.patch
g++-15 -std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic \
  -I/tmp/treeMapGnu17Repro final_results/gnu17/benchReset.cpp -o /tmp/treeMapGnu17Repro/benchReset
/tmp/treeMapGnu17Repro/benchReset --n 1000000 --rounds 5
```
