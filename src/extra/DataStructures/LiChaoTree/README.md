# LiChaoTree

离散坐标上的线段李超树，默认取最大值。Line 提供 Key、Value、const operator[](Key)，对任意两条函数 f、g，沿升序 keys 的 `cmp(f(x),g(x))` 布尔值至多改变一次；仿射直线配合 std::greater/std::less 满足条件。构造复制严格递增坐标，insert(l,r,line) 使用坐标数组下标的半开区间 `[l,r)`，`0 <= l <= r <= n`，空区间无操作；query/queryLine 的坐标必须已登记且被至少一条线覆盖。无任意零直线哨兵，不要求 Line 默认构造。比较器须为严格全序，并满足上述单次切换条件；仅有严格全序并不足够，例如按函数值的绝对值比较可能产生两次切换。计算不得溢出。插入 O(log² n)、查询 O(log n)、空间 O(n+k)，重建清空。完整直线使用 `insert(0,n,line)`；空坐标集可构造并插入空区间，不能查询。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。正确性与性能用例均纳入 CTL，现版验证结果以测试报告为准。
