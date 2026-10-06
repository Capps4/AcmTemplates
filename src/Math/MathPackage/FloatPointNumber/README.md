# FloatPointNumber

[code.hpp](code.hpp)

`FloatPointNumber<T>` 包装浮点值，提供四则运算、EPS 比较、`sgn/val/round/setprecision`；全局 `Float` 为 double 包装。获取原始数值用 `val()`。

EPS 为 1e-12，输出精度须非负；round 在容差内向正无穷方向处理半整数，-1.5 得到 -1。目标类型须容纳舍入结果。精度配置与读入缓存为共享静态状态，按单线程使用。

数学函数在 `std` 中提供包装重载，这是本模板采用的非标准扩展。输入先读字符串，再用 stof/stod/stold 转换；字符串读取失败保留原值。标准流支持浮点输出，Qoutput 只支持整数与字符串。

算术操作 O(1)，格式化和解析成本与文本长度有关。
