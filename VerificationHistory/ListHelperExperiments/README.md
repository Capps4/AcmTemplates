# C++17 sorted 管道

最终排序核心为 75 行：整数 vector 的基数排序，以及通用的 sorted 算子。完整独立头文件 [Sorted.hpp](Sorted.hpp) 为 99 行，包含标准头文件和 Op 管道定义。

```cpp
#include "Sorted.hpp"

std::vector<int> a{3, -2, 1};
auto b = a | sorted();                    // 复制，a 保持原样
a = std::move(a) | sorted();              // 移动复用输入
auto c = std::move(b) | sorted(std::greater<int>{});
```

已有完整 ListHelper 的代码使用 [ListHelperAdaptive.hpp](ListHelperAdaptive.hpp) 或 [可替换的 snippet](ListHelperOptimized.code-snippets)；独立使用 sorted 时选择 Sorted.hpp。这两种头文件是替代版本，不应在同一翻译单元重复定义 seq::Op / sorted。

- 保留左值复制、非 const 右值移动、立即执行并返回实体容器的语义。
- 首尾判向后检查一个方向；已有序直接返回，倒序直接反转。
- 标准整数 vector（含自定义 allocator）识别 std::less<T>、std::less<>、std::greater<T>、std::greater<>，使用基数排序。缓冲沿用结果容器的 allocator。
- unsigned 相对值处理负数与极值，桶计数全部相同时跳过散写。
- 小规模输入回退比较排序；默认 8 位，n >= 4096 且 11 位减少总轮数时选择 11 位。
- bool、其他类型、自定义比较器，以及 array / deque / string 等随机访问容器使用 std::sort；不增加链表支持。

比较器若含不可复制状态，可使用移动传入 sorted(std::move(cmp))；自定义比较器按引用传给标准排序算法，保持既有行为。不承诺稳定排序，也不承诺保存排序前 vector 的存储地址。

测试了 8 / 11 / 15 位。15 位在部分稀疏输入上更快，但一般输入与小数组不稳定，因此未进入默认分派。GCC 的独立百万随机 int 测量中，默认升序从 3.637 ms 降为 2.963 ms，typed less 从 39.436 ms 降为 2.967 ms。全部默认随机验证组的耗时几何平均降低约 6.7%，n >= 4096 的组降低约 10%–11%。不能据此保证任意输入或机器都更快。

分步骤结果、独立验证、回归范围和复现方式见 [测试报告](benchmark/sortedOptimization/RESULTS.md)。VS Code 中已安装的 snippet 保留为基线，新 snippet 文件供替换使用。
