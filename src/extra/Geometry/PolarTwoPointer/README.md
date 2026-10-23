# PolarTwoPointer

将非零 `Vec<T>` 按从正 x 轴开始的极角稳定排序，并返回每个起点逆时针 [0,π) 窗口的首个排除位置（模 n），起点自身不计，其他同向向量计入，对向不计。窗口长度为 (ans[i]-i-1+n)%n。空输入返回空；输入原地重排。依赖 core Geo2 的 PointVec；标量点积、叉积必须可表示，整数建议 long long 且各坐标绝对值≤1e9。浮点依其比较语义，近退化输入不保证精确。排序 O(n log n)，双指针 O(n)，空间 O(n)。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
