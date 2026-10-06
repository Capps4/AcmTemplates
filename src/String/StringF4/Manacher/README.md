# Manacher

[code.hpp](code.hpp)

`Manacher(string_view)` 保存奇/偶回文半径和每个位置结尾的最长回文长度，不保留输入视图，支持任意字节及 NUL。

`getPalinLenFromCenter(i,between)` 返回完整长度，between=false 的中心为 i，true 的中心为 i+0.5；`getPalinLenFromTail(i)` 查询结尾最长长度。`isPalindrome(l,r)` 判断 `[l,r)`，空区间为 true。

普通中心/结尾下标须在 `[0,n)`，半字符中心须在 `[0,n-1)`；空文本只可查询空区间，长度须能用 int 表示。构建与空间 O(n)，查询 O(1)。
