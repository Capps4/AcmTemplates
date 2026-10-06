# LinearBasis

[code.hpp](code.hpp)

异或线性基提供 `insert/check/getMax/getMin/clear/findByOrder`，公开 `b/rank/canZero`。

支持非负 signed 及完整 unsigned 位域；`findByOrder(k)` 为非空子集 distinct XOR 的 0 基排名，k 须有效，零是否可取决于 canZero。允许空子集时按源码注释关闭 `k += !canZero`。

查询排名前按 dirty 状态约化；插入/普通查询 O(bits)，约化 O(bits²)，空间 O(bits)。
