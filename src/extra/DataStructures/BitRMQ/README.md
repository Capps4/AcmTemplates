# BitRMQ

位压缩静态 RMQ，BitRMQ<T,Cmp> 与 core RMQ 名称区分；默认最小值，可用 greater 求最大值。查询非空半开区间 [l,r)，空数组可构造但不可查询。T 可复制，Cmp 为严格弱序。块宽 64，位扫描仅用于非零掩码。预处理 O(n+(n/64)log n)，查询 O(1)，空间同预处理；输入由对象复制。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
