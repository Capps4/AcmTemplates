# Trie

C++17：内部统一放在 `_trie`，算法入口为 `StringTrie`、`BinaryTrie` 两个模板别名。核心管理路径、静态节点池与自动 COW；Info 只存数据，修改和自定义查询用闭包描述，无 push/pull、merge、范围 query 或内置 first/last。

- [Final.hpp](Final.hpp)：核心与字符串、整数路径编码。
- [DemoString.cpp](DemoString.cpp)：插入字符串，查询 lower_bound。
- [DemoBinary.cpp](DemoBinary.cpp)：插入整数，查询最大异或值。
- [Demo.cpp](Demo.cpp)：前缀版本两根查询、树上点权四根查询第 k 小。
- [Test.cpp](Test.cpp)：随机模型对拍、COW 和位宽边界回归。

每个 demo 都有独立 main，分别编译运行。Test.cpp 直接复用 demo 中的查询函数：

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Werror -pedantic Test.cpp -o /tmp/trie-test
/tmp/trie-test
clang++ -std=c++17 -O1 -g -fsanitize=address,undefined Test.cpp -o /tmp/trie-sanitized
/tmp/trie-sanitized
```

头文件尾部提供带 `replaceme` 注释的 `Info` 示例：`count` 表示前缀计数，`end` 表示终点计数，可直接改写。Demo 分别使用 StringInfo、BinaryInfo、PathInfo；测试另用 TestInfo，避免占用示例名称。

## 修改与查询

```cpp
using Tree = StringTrie<Info>;
Tree::clearInit();
Tree tree;
std::string_view key = "apple";
tree.modify(key, [&](Info& info, int dep) {
    ++info.count;
    if (dep == int(key.size())) ++info.end;
});
auto old = tree;
tree.modify(key, [](Info& info, int) { ++info.count; });
auto prefix = old.query("app"); // prefix.count == 1
```

modify(key,f) 在根及整条路径执行 f(Info&,int dep)，根深度为 0，终点深度为键长。空字符串只更新根。回调自行区分前缀计数与终点信息；不要求 Info 提供 apply、get 或加减号。Info 须可默认构造与复制；Info{} 是新节点和零号空节点的默认信息。

query(key) 返回路径终点的 Info 副本，缺失路径返回 Info{}。字符串键也可用于前缀查询；整数键按完整的 Bits 位查找。根信息通过 walk 的 dep==0 回调读取。

## 遍历

| 接口 | 回调签名 |
| --- | --- |
| walk(f) | f(const Node&, int dep) |
| walk(b,f) | f(const Node&, const Node&, int dep) |
| walk(b,c,d,f) | f(const Node&, const Node&, const Node&, const Node&, int dep) |

回调返回下一条边的编号，-1 表示结束；多根始终沿同一条边前进。遇到空节点不会自动结束：两根差分时一个节点为空，另一个仍可能有数据。回调必须保证返回值为 -1 或 [0,degree) 内的编号，并自行在终点停止。二进制查询应先判断 dep==Bits，再计算 Bits-1-dep，避免负移位。

Node 暴露 info 与 son 编号数组，node[d] 返回孩子的 const Node&。零号节点的孩子仍为零号节点。son[d]==0 表示结构缺失，元素是否存在由用户字段决定；删除不回收节点。

```cpp
tree.walk([&](const Tree::Node& u, int dep) {
    // u.info：当前前缀信息；u[d].info：某个孩子的信息。
    if (dep == 0) { /* 可以在此对 u 自定义 DFS，遍历多条分支 */ }
    return -1;
});
```

Node/Info 引用都是临时借用，不能跨可能扩容的写操作保存，也不能把它们当作旧版本句柄。回调持有这些引用期间，不得 modify 或 clearInit 同一池的任何树。要保留旧版本，直接复制树。

## 多根查询

数组前缀版本 prefix[i] 保存前 i 个元素；下标区间 [l,r] 对应 prefix[r]-prefix[l-1]。两根第 k 小的核心写法：

```cpp
unsigned ans = 0;
prefix[r].walk(prefix[l - 1], [&](const auto& a, const auto& b, int dep) {
    if (dep == Bits) return -1;
    const auto cnt = a[0].info.count - b[0].info.count;
    const int d = k > cnt;
    if (d) k -= cnt;
    ans |= unsigned(d) << (Bits - 1 - dep);
    return d;
});
```

树上点权版本 version[u] 保存根到 u（含 u）的路径。令 w=LCA(u,v)，路径集合为 version[u]+version[v]-version[w]-version[parent[w]]：

```cpp
version[u].walk(version[v], version[w], version[parent[w]],
    [&](const auto& a, const auto& b, const auto& c, const auto& d, int dep) {
        if (dep == Bits) return -1;
        const auto cnt = a[0].info.count + b[0].info.count
                       - c[0].info.count - d[0].info.count;
        const int s = k > cnt;
        if (s) k -= cnt;
        ans |= unsigned(s) << (Bits - 1 - dep);
        return s;
    });
```

所有字段组合在回调里完成，Info 不必重载加减号。按计数选择时，各键组合计数须非负，且 1<=k<=总数量。边权路径采用 version[u]+version[v]-2*version[w]；DFS、LCA 和选择算法放在模板外。

## 参数、COW 与复杂度

StringTrie<Info,D=26,First='a'> 使用连续字节字母表；BinaryTrie<Info,UInt=unsigned,Bits=std::numeric_limits<UInt>::digits> 从高位到低位编码。UInt 必须是无符号整数；静态断言要求 radix==2，所以 digits 是二进制有效位数，unsigned 的全部位均为有效位。digits10 才是十进制精度，本模板没有使用它。Bits 必须在 [1,digits] 内。

键及遍历参数的合法性由调用方保证：字符串字节须位于字母表内，长度须能由 int 表示；整数若 Bits 小于完整位宽，应满足 x<2^Bits（核心只读取低 Bits 位）。池编号须能由 int 表示。完整模板实例各有独立静态池，必须先 clearInit(cap=0)；再次初始化会使所有旧树失效。

树仅存 rt、frozen 两个 int。非零编号小于 frozen 时写入复制，否则独占并原地修改；复制非空树同时冻结源与目标的已有节点。移动转移根与边界，源变为空树。query/walk/Node[d] 不冻结、不分配节点。池只追加，无引用计数、shared 标记或析构回收。

设键长为 L、分支数为 D，Info 更新与复制 O(1)：独占路径修改 O(L)，COW 修改 O(DL)，query O(L)，Node[d] O(1)。walk 的循环开销与访问层数成正比，回调额外开销由用户算法决定。核心修改与查找均为迭代；字符串 lower_bound demo 使用递归 DFS。

## 参考

- [Nyaan String Trie](https://nyaannyaan.github.io/library/string/trie.hpp)：固定孩子数组与终止信息。
- [Luzhiled Binary Trie](https://ei1333.github.io/library/structure/trie/binary-trie.hpp.html)：沿路径维护计数，以 xor 参数指定查询顺序。
- [AtCoder ABC353 E 官方题解](https://atcoder.jp/contests/abc353/editorial/10110)：沿路径维护前缀计数。
- [AtCoder ABC377 G 官方题解](https://atcoder.jp/contests/abc377/editorial/11244)：沿路径维护前缀最小值。
