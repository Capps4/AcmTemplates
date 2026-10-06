# Trie

[code.hpp](code.hpp)

`StringTrie<Info,D=26,First='a'>` 与 `BinaryTrie<Info,UInt=unsigned,Bits=digits>` 管理路径和共享节点池，Info 只存数据，操作由闭包定义。

创建前调用 `clearInit(cap=0)`；再次初始化使旧树失效。`modify(key,f)` 在根和每层执行 `f(Info&,dep)`，根 dep=0，空字符串只更新根；`query(key)` 返回终点 Info 副本，缺失为 Info{}。

`walk(f)`、`walk(b,f)`、`walk(b,c,d,f)` 沿多根相同边前进，回调接收对应 const Node 引用及 dep，返回边号或 -1 停止。Node 提供 info、son 和孩子访问；缺失节点不能直接视为多根组合查询结束，二进制查询先判断 dep==Bits 再移位。

复制树自动保护旧节点，修改采用 COW，移动后源为空。Info 须可默认构造/复制；回调不得重入修改同池树，节点引用不能跨扩容保留，池只增长。

字符须在连续字节域内；UInt 为无符号整数，Bits 在 [1,digits] 内，较小位宽只读取低 Bits 位，键及池编号须适合 int。键长 L，独占修改 O(L)、COW 修改 O(DL)、查询 O(L)，回调成本另计。

[使用示例](../../../../tests/Correctness/DataStructures/TreeDataStructures/Trie/Demo.cpp)。
