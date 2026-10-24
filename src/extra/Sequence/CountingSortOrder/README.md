# CountingSortOrder

[code.hpp](code.hpp)

`countingSortOrder(a,key)` 返回按非负整数键稳定升序排列的原下标，空输入返回空序列。

键投影须稳定，元素数量可由 int 表示，键满足 `0 <= key < INT_MAX`。`INT_MAX` 会在分配前抛出 `length_error`，避免桶数和循环下标溢出。时间与空间 O(n+maxKey)；键域很大且稀疏时改用比较排序。
