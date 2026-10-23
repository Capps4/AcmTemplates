# ListHelper

[code.hpp](code.hpp)

Op 管道提供 `sorted/unique/reverse/slice/filter/map/enumerate`，终止操作为 `count/first/accumulate`；成员调用用 `call(name,...)` 或 `seq::memberCall`。

```cpp
auto b = a | sorted() | unique();
auto ids = b | enumerate();
```

左值列表复制，右值消费；string_view 先转成 string。slice 使用截断的 `[l,r)`；整数 vector 在适用时走基数排序，其他比较器使用标准排序。

成员调用对左值 const 借用、右值消费；临时对象的引用结果会复制，pointer/view/iterator 的寿命由调用方保证。流管道使用标准流，FastInputOutput 的同名宏应最后 include。

遍历操作通常 O(n)，比较排序 O(n log n)，整数基数排序成本与有效位数有关；复制成本另计。
