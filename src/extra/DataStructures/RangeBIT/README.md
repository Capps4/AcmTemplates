# RangeBIT

`RangeBIT<T>(n)` 零初始化，或从数组 O(n) 构建；rangeModify(l,r,v) 区间加，rangeQuery(l,r) 区间和，均为 0 基半开 [l,r)。空区间合法，支持 n=0。T 支持默认零、加减、乘下标，所有值与中间乘积须可表示。依赖 core Fenwick；操作 O(log n)、空间 O(n)，重建清空。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
