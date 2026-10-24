# SlopeTrick

`SlopeTrick<T=long long>` 表示初始恒为 0 的凸分段线性函数。`T` 支持零初始化、排序和有符号加减；坐标、偏移、函数最小值及运算中间值都须可表示。使用浮点数时要求有限值，由调用方承担其比较与累计误差。不使用无穷大哨兵。

- `addConstant(c)`：加常数；`addLeft(a)` 加 `max(a-x,0)`，`addRight(a)` 加 `max(x-a,0)`，`addAbs(a)` 加 `abs(x-a)`，斜率权重均为 1。
- `shift(d)`：得到 `f(x-d)`；`shift(l,r)` 要求 `l <= r`，得到 `min f(y)`，其中 `x-r <= y <= x-l`。
- `prefixMin()`：得到 `min(y<=x) f(y)`；`suffixMin()`：得到 `min(y>=x) f(y)`。
- `minimum()`：返回 `{一个最优点, 最小值}`。恒函数返回点 0；不枚举全部最优区间。

设保留的折点数为 `k`，添加 hinge/绝对值为 `O(log k)`，平移和读取最小值为 `O(1)`；前后缀最小化清空一个堆，释放最多 `O(k)` 个元素。空间 `O(k)`。对象拥有状态，复制互不影响；重新构造即重置。不提供任意权重、求值或函数合并。

思路参考 [Nyaan's Slope Trick](https://nyaannyaan.github.io/library/data-structure/slope-trick.hpp.html)，参考库许可为 [CC0-1.0](https://github.com/NyaanNyaan/library/blob/master/LICENSE)。本实现独立编写，仅保留单位斜率的双堆表示，未复制参考库代码。
