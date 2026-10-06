# DeletableHeap

[code.hpp](code.hpp)

`DeletableHeap<T,Cmp>` 用两个堆实现惰性删除，提供 `push/emplace/erase/top/pop/size/empty`，支持 vector 批量建堆与自定义比较器。

`erase(x)` 要求当前存在 x，每次删除一次出现；比较器等价关系须与 `==` 一致。比较器须可复制且顺序固定。`top()` 返回借用引用，修改或同步后丢弃旧引用。

批量建堆 O(n)；插入/删除登记 O(log n)，同步的额外成本按每个历史项最多弹出一次摊还。内存包含尚未弹出的历史项。
