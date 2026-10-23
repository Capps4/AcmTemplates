# WeightedDisjointSetUnion

`WeightedDSU<T>(n)`，顶点 [0,n)，w[x] 是到当前父亲的势差；find(x) 后是到根的势差。Union(x,y,z) 约束 potential[y]-potential[x]=z，返回是否一致；失败不改变已有关系。T 是交换加法群，支持默认零、+、-、==，运算不得溢出。按大小合并和路径压缩，均摊 O(alpha(n))、空间 O(n)，无递归。重建对象清空关系。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
