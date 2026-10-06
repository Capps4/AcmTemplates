# Discreter

[code.hpp](code.hpp)

`discreteFrom(basis)` 将输入值映射为已升序 basis 中的 `lower_bound` 插入位置：

```cpp
auto basis = a | sorted() | unique();
auto ids = a | discreteFrom(basis);
```

basis 左值借用、右值拥有；借用时保证生命周期及排序不变。值不存在也返回插入位置。n 个值、m 个基准的查询时间 O(n log m)。
