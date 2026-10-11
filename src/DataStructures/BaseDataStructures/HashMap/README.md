# HashMap

[code.hpp](code.hpp)

`HashMap<Val,Capa>` 使用共享节点池、owner 隔离及常量模取桶；内部实现为 `_hashmap::Impl/Pool`。

`map[key]` 缺失时插入，返回可写引用；`map(key)` 只读返回 `std::optional<Val>` 值副本，缺失为 `std::nullopt`，不插入。查询和迭代器返回值副本，要求 Val 可复制；修改副本不写回容器。存储的 `0` 或 `false` 仍表示键存在。引用在修改或池回收后可能失效。

公开 `key_type = u64`、`mapped_type = Val`，供通用映射识别；调试打印沿用迭代器的值复制要求。

相同 Val/Mod/Capa 的对象共享池；多对象 clear 切换 owner，唯一对象 clear 或最后对象析构回收全池。容量包含所有未回收节点，池满会报错；按单线程使用。

GCC GNU++17 支持默认编译期随机模数；Clang C++17 使用显式 `_hashmap::Impl<Val,Mod,Capa>`。查询成本取决于桶链长度，均匀分布时平均 O(1)，最坏线性。
