# TwoSat

[code.hpp](code.hpp)

`TwoSat(n)` 使用 `2*x+value` 编码。`add(x,f,y,g)` 表示 x=f 蕴含 y=g，并加入逆否命题；`assign` 固定变量。

`work()` 通过 SCC 求解，成功发布 ans，失败清空 ans，重复调用重新生成答案。变量从 0 开始，时间/空间 O(n+m)；依赖 SCC 的递归栈，长蕴含链需足够栈容量。
