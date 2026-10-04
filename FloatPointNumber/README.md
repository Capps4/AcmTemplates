# 浮点包装（GNU++17）

`FloatPointNumber<T>` 保存一个浮点标量，保留四则运算、EPS 比较、sgn/val/round/setprecision。普通数可通过构造函数隐式包装；包装数没有向普通数的转换运算符，读取原始值使用 `.val()`。因此 `x + 2`、`2 + x`、`x == 0` 直接使用包装类运算，不需要额外混合运算重载宏。

按用户要求，直接为 `std::abs/sqrt/sin/cos/tan/asin/acos/atan/atan2` 添加包装类重载。一元函数由 `FLOAT_POINT_UNARY_FN` 生成，随后 undef；atan2 单独定义。内部将 `.val()` 交给标准库计算，返回相同底层 T 的包装类型。普通浮点数仍调用原标准库重载；无需在调用处区分类型。该 namespace std 扩展是本模板有意采用的非标准接口。类内 PI 与重复的 friend 数学函数已删除。

纯算术支持 constexpr，precision 与 inputStr 为 inline static。round 使用 std::round，调用方保证结果能放进目标类型；precision 需非负。竞赛用途按单线程使用，不再包含 thread_local 或缓存搬运逻辑。

输入沿用原版的字符串方案：先读入 inputStr，再按 T 使用 stof/stod/stold。标准流与 QInput 都可读入；字符串读入失败时直接返回并保留原值，转换行为遵循相应标准函数，不额外检查整段格式或设置流失败状态。

输出直接使用 fixed/setprecision 写入底层值，不判断输出流类型。标准输出流支持包装类的 operator<<；QOutput 调用处使用 writeReal(a.val(), precision)，不由包装类适配。先输出再返回具体 Output 引用，保留 ostringstream/ofstream 的返回类型。

全局别名 `Float = double`，包装类型显式使用 `FloatPointNumber<double>`。Original 与历史 Results.json / Performance.md 保留为档案，不代表本次接口修改后的验证结果。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check FloatPointNumber`；性能用 `python3 template/Run.py bench FloatPointNumber`。流程说明见 [主 README](../README.md)。本次修改仅做源码差异 Review，未运行编译或测试。
