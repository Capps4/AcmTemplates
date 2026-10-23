# PartialOrder3D

partialOrder<3,T>(a,w) 返回各点三个坐标均≤该点的权重总和，包含自身及全部重复点。输入无需排序，权重可负；T 支持无溢出的加减和默认零。坐标为 int，先合并重复点、再 CDQ，使用 core Fenwick。时间 O(n log² n)、空间 O(n)，无共享状态。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
