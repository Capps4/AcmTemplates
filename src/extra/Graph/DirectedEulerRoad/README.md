# DirectedEulerRoad

有向图顶点 [0,n)，每条边在起点邻接表登记一次，支持自环、重边和孤点。roadType 为 NoSolution/EulerPath/EulerCircuit；ans 为正向顶点序列，无解为空。无边非空图返回一个顶点，空图返回空环。检查度数和全部边是否走完；显式 Hierholzer，O(n+m) 时间与空间，不修改输入。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
