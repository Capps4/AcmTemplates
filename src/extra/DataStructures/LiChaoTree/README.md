# LiChaoTree

离散坐标上的线段李超树，默认取最大值。Line 提供 Key、Value、const operator[](Key)，任意两条函数至多相交一次；仿射直线满足条件。构造复制严格递增坐标，insert(l,r,line) 使用坐标数组下标的闭区间，l>r 忽略；query/queryLine 的坐标必须已登记且被至少一条线覆盖。无任意零直线哨兵，不要求 Line 默认构造。比较器为严格全序，计算不得溢出。插入 O(log² n)、查询 O(log n)、空间 O(n+k)，重建清空。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
