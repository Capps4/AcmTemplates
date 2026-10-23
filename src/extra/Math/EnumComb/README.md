# EnumComb

EnumComb(m,n) 按数值升序枚举 m 位中恰有 n 个 1 的 uint64_t，0≤n≤m≤64。n=0 和 n=m 均只产生一个结果；支持 64 位边界，不移位 64 位。每次递增 O(1)，额外空间 O(1)，无共享状态。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
