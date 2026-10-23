# MatBIT

n×m 二维树状数组，modify(x,y,v) 点合并；posQuery(x,y) 查询闭前缀，-1 边界为零；rangeQuery(u,l,d,r) 查询 [u,d)×[l,r)。默认加法且区间查询要求加法群；自定义 Merge 仅支持带默认零单位元的交换幺半群前缀查询。空维度可构造。所有坐标、运算须在范围内，操作 O(log n log m)，构造/空间 O(nm)。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
