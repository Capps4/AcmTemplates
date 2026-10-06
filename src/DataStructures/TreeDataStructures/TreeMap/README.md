# TreeMap

[code.hpp](code.hpp)

`TreeMap<K,V,Cmp>` 使用在线 SBT，`TreeMapOff<K,V,Cmp>` 使用预登记候选键与 Fenwick，共享有序映射接口。

`[]` 缺失时插入默认值；`operator()(key)` 只读返回 optional 值副本，缺失为 nullopt。`insert/insertOrAssign/erase/contains` 管理元素；`rankOf(key)` 为严格更小元素数，`keyAt(k)` 使用 0 基排名。bool 的 false 仍表示已存在。

在线默认自动扩容，构造指定容量后为固定容量，reserve 可扩充；离线构造仅登记候选，不插入，写未知键报错。Cmp 须固定且满足严格弱序。Key/Value 须满足所用操作的默认构造、复制、移动要求，`()` 要求 V 可复制；bool 元素使用代理。

遍历用 `auto [key,value]`，value 可写回。结构修改、clear、扩容、赋值或移动后丢弃旧迭代器、引用和代理；optional 副本独立。节点池容量与排名须适合 int。

SBT 仅插入重平衡，查询/删除 O(h)，不保证删除后的 h=O(log size)，完整遍历 O(N)。离线增删/查询/排名 O(log U)、遍历 O(U)、初始化排序 O(U log U) 上界；U 为候选键数。

[使用示例](../../../../tests/Correctness/DataStructures/TreeDataStructures/TreeMap/Demo.cpp)。
