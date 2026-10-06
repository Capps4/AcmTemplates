# Geo3

[code.hpp](code.hpp)

区分 `Point<T>/Vec<T>` 和 `Seg<T>/Line<T>`：点减点为向量，点可平移；向量可缩放，提供 `dot/cross/triple`。二维和三维模板导出的类型同名，组合使用时自行区分。

Line 保存点 p 与方向 v，可由两点或 `fromVec(p,v)` 构造；Seg 保存 a/b，允许零长度。`Plane/Hit/Face/Polyhedron` 提供平面交点、三角面面积与多面体体积；foot 需要浮点坐标，直线方向和平面法向须非零。

整数点积/叉积使用提升整数，三重积使用 GNU 128 位；坐标差、乘积及公开结果须可表示。浮点比较沿用 T，整数距离返回 Float。基础向量操作 O(1)，多面体面积/体积与面数成正比。
