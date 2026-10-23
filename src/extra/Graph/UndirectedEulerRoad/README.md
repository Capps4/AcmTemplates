# UndirectedEulerRoad

XorGraph(n).add(x,y) 登记无向边，边 ID 在两端邻接表各出现一次，自环出现两次。UndirectedEulerRoad 可直接 add 或复制 XorGraph 后 work()；不消耗邻接表，允许反复 work 及追加边。roadType 为 NoSolution/EulerPath/EulerCircuit，ans 为正向顶点序列；无边非空图返回一个顶点，空图为空环。度数及全边遍历检查，显式 Hierholzer，O(n+m) 时间与空间。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
