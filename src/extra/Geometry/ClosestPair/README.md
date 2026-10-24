# ClosestPair

`closestPair<T,G>(vector<Point<T>>)` 返回最近点对距离平方，重复点返回 0，少于两点返回 `numeric_limits<G>::max()`。输入按值复制；依赖 core Geo2 PointVec。先转 G 再减坐标，差值平方及其和须可表示；整数精确结果可指定 long long，浮点结果受精度约束。

先按 x 排序，分治返回时归并维护 y 序；共享一个辅助数组，合并后只比较中线条带内的相邻候选。时间 O(n log n)，额外空间 O(n)。

算法参考：[Closest pair divide and conquer](https://cp-algorithms.com/geometry/nearest_points.html)。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。正确性与性能用例均纳入 CTL，现版验证结果以测试报告为准。
