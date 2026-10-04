# TreeMap：GNU++17 / C++17 竞赛有序映射

[Final.hpp](Final.hpp) 复用工程中的 [ListHelper 实现](../ListHelper/Final.hpp)，使用时需保留两者的相对路径或调整 include。该 ListHelper 保留已合入的排序改进和仓库中的所有权修复。`TreeMap<Key, Value, Compare>` 使用在线 SBT；`TreeMapOff<Key, Value, Compare>` 使用离散化后的计数 Fenwick。两者共享接口，类型在编译时确定，不做运行时后端切换。实现集中在 `_treemap` 命名空间，文件末尾仅用 `using` 导出 `TreeMap` 和 `TreeMapOff`。

竞赛中可用 `g++ -std=gnu++17 -O2 main.cpp` 编译；也支持严格的 `-std=c++17`。实现用 `if constexpr` 在编译期选择插入模式、Value 清理策略和键排序算法，配合 `_v` 类型特征省去运行时判断；`decltype(auto)` 保留访问结果的引用或代理类型，遍历可直接使用结构化绑定。

```cpp
#include "Final.hpp"
#include <cassert>
#include <vector>

int main() {
    TreeMap<int, int> map;
    map[5] = 7;
    map[2] += 3;
    assert(!map(9) && map.size() == 2); // 缺失读取返回 nullopt，不插入
    if (auto value = map(5); value)
        assert(*value == 7); // optional 保存独立副本
    assert(map.rankOf(5) == 1);
    assert(map.keyAt(0) == 2);
    for (auto [key, value] : map) value += key;

    TreeMapOff<int, bool> marks(std::vector<int>{2, 5, 9, 5});
    marks[5] = true;
    marks[5] = false;
    assert(marks.contains(5)); // false 也是已存在的值
    marks.erase(5);
}
```

## 构造与容量

- `TreeMap<K,V> map;`：初始为空，节点池按需扩容。
- `TreeMap<K,V> map(n);`：预分配固定容量，最多同时存在 `n` 个不同键；满时新增键抛 `std::length_error`。删除槽会复用。
- `map.reserve(n)`：仅在线版本提供。可扩大预分配容量；固定模式显式扩容后仍保持固定模式。
- `TreeMapOff<K,V> map(keys);`：通过 `std::move(keys) | seq::sorted(...)` 排序，再去重建立候选键空间，**初始仍为空**。候选键必须覆盖后续所有写入；写入未知键抛 `std::out_of_range`。查询未知键合法，删除未知键返回 false。

容量、元素数和排名使用 `int`，按竞赛规模假定容量及自动倍增后的容量都能用 `int` 表示，不做超大容量检查或饱和扩容。Fenwick 更新下标使用 u32，在此容量约束下最后一次步进最多达到 2^31，不会溢出。`clear()` 清空元素并保留已分配容量；离线版本同时保留候选键空间。

当 Value 同时满足 `std::is_trivial_v` 和 `std::is_trivially_move_assignable_v`（例如 int、bool、普通整数数组）时，删除或 clear 不再写回不可访问的旧值。新增键始终显式初始化，所以随后 `[]` 仍得到默认值。SBT 对这类 Value 的 clear 为 O(1)；离线版本仍需 O(U) 清空计数和存在标记。其余 Value 保留原来的重置与资源释放行为。

## 接口

| 接口 | 含义 |
| --- | --- |
| `insert(key, value)` | 新增返回 true；键已存在时保留旧值并返回 false |
| `insertOrAssign(key, value)` | 新增返回 true；覆盖已有值返回 false |
| `erase(key)` | 删除成功返回 true，否则 false |
| `contains(key)` | 查询键是否存在，不读取值 |
| `map[key]` | 可写；缺失时插入默认值 `Value{}` |
| `map(key) -> std::optional<Value>` | 只读；存在时返回值的副本，缺失时返回 nullopt，不插入 |
| `rankOf(Key key) -> int` | 按比较器严格排在 key 前面的元素数；key 不必存在 |
| `keyAt(int k) -> Key` | 返回第 k 个键的副本，**0-based**；负数或越界抛 `std::out_of_range` |
| `size() / empty()` | 元素数 / 是否为空 |
| `clear()` | 清空元素，保留容量 |
| `begin() / end()` | 正向遍历，也支持 const 容器 |

默认比较器为 `std::less<Key>`；自定义比较器须满足严格弱序，排序与键等价均由它决定。等价键保留首次插入时的实际键。

值直接存于 `std::vector<Value>`：`[]` 返回其 `reference`；普通 Value 对应可写引用，bool 使用标准库原生代理，不保证 `bool&`。迭代器也保留引用或代理，不复制整个 Value。

`()` 返回 `std::optional<Value>`，命中时复制 Value，缺失时返回 `std::nullopt`，不会默认构造 Value 或插入键。返回结果拥有自己的值，修改副本或随后删除、清空、扩容 Map 都不影响它。调用 `()` 要求 Value 可复制；unique_ptr 等不可复制 Value 仍可用 `[]`、contains 和迭代器，其余增删/移动能力保留。

```cpp
marks[5] = false;
auto value = marks(5); // optional<bool>{false}
assert(value.has_value() && !*value);
assert(!marks(2));     // nullopt，键不存在
bool flag = marks(5).value_or(false);
```

**`if (marks(key))` 判断键是否存在；`*value` 才是 bool 值。** 写入 false 不会删除键，也不会改变排名。读取缺失时需要默认值，可用 `map(key).value_or(Value{})`；如果只判断存在性，contains 不会复制 Value。

迭代器只提供 `*it`、前置 `++it`、`==` 和 `!=`。`*it` 返回临时视图，使用示例中的 `auto [key, value]`：key 只读，非 const 容器的 value 可通过引用或 bool 代理写回。SBT 的祖先路径保存在迭代器中，上限 128 层；节点不增加父指针。离线迭代器保存容器指针和坐标下标。

新增、删除、清空、扩容以及容器赋值或移动后，应丢弃旧迭代器、元素引用和代理；`()` 返回的 optional 副本不受影响。仅修改已有值不会改变结构。`end()` 不可解引用或递增。

## 实现与复杂度

SBT 的 int 键节点是 **16 字节**：一个键、两个孩子下标和子树大小。Value 存在独立数组中，其大小不影响节点布局。SBT 的 Key 以及两个版本的 Value 需要默认构造，并支持所用操作的复制、移动及赋值；复制容器还要求元素可复制。

SBT 仅在插入时维护平衡，**删除不重平衡**。以当前树高 h 表示，查询和删除为 O(h)，插入另有平衡维护与可能的扩容成本；不保证相对当前 `size()` 的严格 O(log size)。SBT 完整遍历为 O(N)。`()` 命中时还需复制 Value，其成本取决于 Value 类型。键和值的搬运、赋值按不抛异常的竞赛使用场景设计。

离线版本只用 Fenwick 维护各键的 0/1 存在计数，不聚合 Value，因此直接修改值不会影响排名。增删、contains、rankOf 各需一次二分定位原始 Key；keyAt 直接做 Fenwick 选择。设去重后的候选数为 U，单次增删、查询和排名选择为 O(log(U+1))；完整遍历扫描候选空间，为 O(U)。候选输入 vector 的容量可能大于 U，尤其输入含大量重复键时。

离线版只保留一份键数组：首次实际插入时用等价 Key 替换候选代表，不破坏排序。Key 移动赋值应保持值语义且不抛异常；失败赋值可能破坏候选排序。构造只注册候选，不是批量插入，不能把随后逐次插入的耗时等同于已知初始计数的 O(U) Fenwick 建树。

TreeMapOff 不再实现自己的排序算法，构造时通过 `std::move(keys) | seq::sorted(compare)` 把候选 vector 交给 ListHelper。标准整数升降序比较器都能进入基数路径，自定义比较器按原样传入，不再先升序排序后反转。直接写 `keys | sorted()` 会复制左值输入。

共享 sorted 先检查已有序或反向有序。非 bool 整数配合标准 less/greater 比较器，在 `n >= 256` 且 `n >= 256 × P8` 时进入基数排序，P8 由 max-min 的有效位数决定。默认每轮 8 位；`n >= 4096` 且 11 位能减少轮数时改用 11 位。恒定桶跳过散射；结果停留在临时缓冲时回写一次。其他输入回退到 std::sort。临时键缓冲为 O(M)，计数桶为 256 或 2048 个 size_t。最新对照见 [sorted 与旧 sortKeys 的测试报告](final_results/currentSortedVsSortKeys/RESULTS.txt)。

## 验证与跑分

在本目录运行：

```bash
bash runBenchmark.sh
# 可覆盖默认规模与轮数
BENCH_N=100000 BENCH_Q=200000 BENCH_ROUNDS=5 bash runBenchmark.sh
```

脚本验证 TreeMap（含 ListHelper 离线候选排序）的 GCC GNU++17、GCC 严格 C++17、Clang 严格 C++17 和 ASan/UBSan，再用 GNU++17 串行运行基准；默认 N=Q=1,000,000、离线候选 U=2,000,000、测量 5 轮。编译器可通过 `GCC_CXX`、`CLANG_CXX` 指定，随机种子可通过 `BENCH_SEED` 指定。

性能说明见 [PERFORMANCE.md](PERFORMANCE.md)，容量选择与回退说明见 [容量报告](final_results/capacity/README.md)，接口精简与同进程对照见 [精简报告](final_results/apiSimplification/README.md)，此前独立排序的历史实验见 [排序报告](final_results/adaptiveSorting/README.md)，此前构建优化的对照见 [历史构建报告](final_results/buildOptimization/README.md)，GNU++17 清理策略的前后对照见 [专项报告](final_results/gnu17/README.md)，环境、原始数据和验证日志见 [final_results](final_results/)。性能倍数以这些实测结果为准。

## 同步后的验证范围

本目录保留原 `check.cpp`、`benchmark.cpp` 和 `runBenchmark.sh`；在本目录执行 `bash runBenchmark.sh`。历史 `final_results/` 保留原测量时的源码路径和摘要；本次迁移未重新测量。当前 Manifest 暂缓本模块进入统一 Run.py 流程。
