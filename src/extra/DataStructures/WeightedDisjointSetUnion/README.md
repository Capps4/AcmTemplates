# WeightedDisjointSetUnion

`WeightedDSU<T>(n)`，顶点 [0,n)，w[x] 是到当前父亲的势差；find(x) 后是到根的势差。Union(x,y,z) 约束 potential[y]-potential[x]=z，返回是否一致；失败不改变已有关系。T 是交换加法群，支持默认零、+、-、==，运算不得溢出。按大小合并和路径压缩，均摊 O(alpha(n))、空间 O(n)，无递归。重建对象清空关系。

尾部 `// replace me` 的 `Info` 是整数加法群示例，可直接使用 `WeightedDSU<Info>`。替换时保留默认零、`+`、`-`、`==` 的交换群语义。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。正确性与性能用例均纳入 CTL，现版验证结果以测试报告为准。
