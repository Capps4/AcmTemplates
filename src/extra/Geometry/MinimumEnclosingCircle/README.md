# MinimumEnclosingCircle

`minimumEnclosingCircle(ps, seed, eps)` 返回 `optional<Circle<T>>`；空集为 nullopt，单点及全重合点半径为 0。输入按值复制，可移动传入；点序不会写回调用方。seed 可指定以复现打乱顺序，默认使用时钟种子。

复用 core Geo2 的 Point、Circle；T 支持原生浮点和 FloatPointNumber，整数坐标先转换。允许重合、共线；坐标、中间平方与外接圆计算须有限且可表示，长度须适合 int。eps 是有限非负的距离平方相对容差，默认 1e-12；最后重新计算覆盖半径。近退化或数值条件差的输入仍受浮点误差影响，不承诺精确几何。

随机增量维护由至多三个点确定的圆。均匀随机排列下期望 O(n)，最坏 O(n³)，含输入副本空间 O(n)，无递归。算法参考：[cp-algorithms](https://cp-algorithms.com/geometry/enclosing-circle.html)。
