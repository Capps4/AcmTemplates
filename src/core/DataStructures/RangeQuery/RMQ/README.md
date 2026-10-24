# RMQ

[code.hpp](code.hpp)

`RMQ<T,F>(a,f)` 使用稀疏表，`rmq(l,r)` 按原数组顺序合并非空区间 `[l,r)`，结果按值返回。C++17 可直接写 `RMQ rmq(a,f)`。

`F` 默认是 `const T& (*)(const T&,const T&)`，支持 `RMQ rmq(a,std::min<int>)`、`RMQ rmq(a,std::max<int>)`，其中 a 为 `std::vector<int>`。GCD 可写 `RMQ rmq(a,std::gcd<int,int>)`；lambda 和其他函数对象仍推导为各自类型。默认的是类型，构造时仍须传入 `f`。

`f` 须满足结合律 `f(f(x,y),z)==f(x,f(y,z))` 和幂等律 `f(x,x)==x`，不要求交换律，也不需要单位元。min/max、非负整数 gcd、按位 AND/OR 均可使用；仅有幂等律不足以保证正确。重叠查询段保持左右顺序，重复的中间段依幂等律消去。

数组长度须可由 `int` 表示。空数组允许构造，不能查询；查询下标须合法。`T` 须可默认构造、复制构造与复制赋值；`f` 的结果须可转换为 `T`，并对 `T` 的合法值封闭。`F` 按值接收后移动保存，可传 move-only callable；须可经 const 对象调用，且同一输入的结果在对象生命周期内保持一致。输入数组由对象复制，无更新或重置接口。

按 `f`、`T` 复制为 O(1) 计，建表时间/空间 O(n log n)，查询 O(1)。
