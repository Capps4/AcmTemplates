# DisjointSetUnion

[code.hpp](code.hpp)

`DSU(n)`、`init(n)`、`find(x)`、`Union(x,y)`；顶点从 0 开始，分量大小为 `size[find(x)]`。

find 使用路径折半；Union 将 y 的代表接到 x 的代表，保留合并方向，不按大小交换。初始化 O(n)、空间 O(n)；单次操作取决于当前树高，不按按秩合并版本承诺 α(n) 的摊还界。
