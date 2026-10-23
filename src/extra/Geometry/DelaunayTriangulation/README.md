# DelaunayTriangulation

保留逐点插入、面桶定位与翻边的原算法。Delaunay 接受 `vector<Point<long double>>`，输入编号保留；ps 尾部三个点为包围三角形。遍历 face 中半边的 r 三步获得三角形，仅保留三个 from 均<n 的面。vertex/face 为借用内部池的指针，不得在对象销毁后使用；对象禁止复制，内存由 deque 自动释放。

支持域：有限坐标且绝对值<1e8，点互异、无三点共线、无四点共圆，且输入不得落在插入过程中边上。浮点近退化输入不保证可靠；定位失败抛 domain_error，成功返回也不等于通过精确几何验证。少于三点返回空面集。最坏时间 O(n²)、空间 O(n)；原实现未随机化，不宣称期望 O(n log n)。测试使用独立行列式空圆检查、凸包边数和面数检查，尚待运行。

来源：[extra_template](https://www.yuque.com/capps/template/library_extra)。迁移后的实现与测试待执行严格验收。
