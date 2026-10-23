# TreeDiameter

非负整数权连通无向树的直径。邻接元素为 int（边权 1）或 pair<int,W>（W 须能无损转 long long）；所有路径和须适合 long long。空树 v1=v2=-1、diameter=0；零权边合法。显式栈两次遍历，O(n) 时间与空间。对象借用邻接表，输入必须存活；不接受有环图或森林。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
