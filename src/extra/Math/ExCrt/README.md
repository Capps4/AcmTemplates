# ExCrt

有符号至多 64 位整数；insert(a,m) 要求 m>0。get() 消耗全部方程，返回最小非负解，无解为 -1；合并模数超出 T 时抛 overflow_error，重复 get() 返回 0。宽整数保护中间乘法。k 个方程时间 O(k log M)，空间 O(k)。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
