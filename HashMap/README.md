# HashMap

以 Original 的共享池实现为基础，只做少量 GNU++17 整理。实现及 u32/u64 收在 `_hashmap` 中，对外仅导出 `HashMap`。保留原来的常量模取桶、owner 隔离、连续 run 遍历、稀疏/稠密清空和移动后源对象可复用的行为。

```cpp
HashMap<int, 100000> map;
map[7] += 3;
if (auto value = map(7))
    std::cout << *value;
for (auto [key, value] : map)
    std::cout << key << ' ' << value << '\n';
map.clear();
```

接口只保留构造/移动、`[]`、`()`、`clear()` 和 `begin()/end()`。不维护 count，不提供 size、empty、find、contains、cbegin 或 cend。

Iterator 恢复 Original 的简单结构，只支持解引用、前置递增和相等比较。解引用返回 `std::pair<u64, Val>` 副本，const 与非 const 容器使用同一种迭代器；修改遍历得到的 value 不会改动 Map，需要修改时用 `map[key]`。不提供箭头代理、后置递增、iterator traits 或 const 转换。遍历要求 Val 可复制。

`operator()(u64 key) const` 返回 `std::optional<Val>`：命中时复制值，缺失时返回 nullopt，不插入。值为 0 或 false 时仍然有值。需要默认值时写 `map(key).value_or(Val{})`，返回的副本不受之后修改或清空 Map 影响。不可复制 Val 可以使用 []，但不支持 () 或解引用迭代器。

删除 owner 溢出及池满的运行时检查，由使用者保证总节点分配不超过 Capa、owner 编号不溢出。同 Val/Mod/Capa 的所有 Map 共享一个池；多个 Map 同时活跃时 clear 只换 owner，不回收旧节点。最后一个 owner 析构或唯一 owner clear 才回收全池。非平凡析构的 Val 会重置，类型约束用 `noexcept(value = Val{})` 合并检查默认构造、赋值及临时对象析构。

编译随机桶数恢复为 `std::sqrt`，当前 GCC 15 的 GNU++17 可将它用于模板常量；当前 Clang 17 的 C++17 不支持此常量表达式，需要显式使用 `_hashmap::HashMapImpl<Val, Mod, Capa>`。保留单翻译单元竞赛用途，不扩展线程安全。桶哈希仍为 `(key + ownerShift) % Mod`。

验证和交付直接使用 Final.hpp。Test.cpp 核对十五万随机交错操作、多 owner、清空/移动、完整遍历、全碰撞、填满后复用、资源释放和 optional 读取。Clang sanitizer 使用显式桶数验证核心。

Results.json、Benchmark.csv 和 Performance.md 已按当前简洁版重跑，包含 GCC、Clang sanitizer 与七轮性能原始样本。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check HashMap`；性能用 `python3 template/Run.py bench HashMap`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
