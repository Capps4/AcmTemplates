# ClosestPair

`closestPair<T,G>(vector<Point<T>>)` 返回最近点对距离平方，重复点返回 0，少于两点返回 `numeric_limits<G>::max()`。输入按值复制；依赖 core Geo2 PointVec。先转 G 再减坐标，差值平方及其和须可表示；整数精确结果可指定 long long，浮点结果受精度约束。保留分治与条带排序流程，O(n log² n) 时间、O(n) 额外空间。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
