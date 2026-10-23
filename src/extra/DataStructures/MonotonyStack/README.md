# MonotonyStack

默认返回前一个严格更小元素的下标；构造时传入哨兵值与哨兵下标，哨兵必须对所有输入不被弹出，例如输入≥0 时用 (-1,-1)。Cmp 可替换；重复值按比较器弹出。均摊插入 O(1)、总空间 O(n)。不提供 reset，重建对象开始新序列。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
