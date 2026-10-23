# BiGraphColor

无向图顶点 [0,n)，各无向边在邻接表两端登记。col 为 0/1 染色；isBiGraph=false 时部分顶点可能尚未着色。支持空图、孤点、重边，自环判失败。显式 BFS，O(n+m) 时间、O(n) 空间，不借用邻接表。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
