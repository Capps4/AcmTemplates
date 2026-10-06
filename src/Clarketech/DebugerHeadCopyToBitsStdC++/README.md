# DebugerHeadCopyToBitsStdC++

[code.hpp](code.hpp) 提供 C++17 调试器；[Debuger](../Debuger/README.md) 提供题解中的启动片段。

先包含所有标准头及调试器，再使用原有配置：

```cpp
#include "code.hpp"
auto $ = strdup("color: false, space: false, precision: 6");
#ifndef COMPETITION_DEBUGER
    #define debug(...)
#endif

int main() {
    int a = 3;
    std::vector<int> v{1, 2};
    debug(a, v); // 例如：  10: a, v = 3, [1, 2]
    free($);
}
```

本地也可通过 `-include /绝对路径/code.hpp` 统一加载，GCC、Clang 均使用这份源码；题解只保留启动片段。若习惯安装到 `bits/stdc++.h` 末尾，复制生成的代码块并替换旧实现。重复包含受宏保护；每个翻译单元均需加载同一实现，启动配置只写在主程序中。

## 配置与输出

- `color`、`space` 接受 `true/false`，默认均为 false；`precision` 为 0–30，默认 6。键和值忽略大小写及首尾空白；无效配置抛 `std::invalid_argument`，不覆盖已有配置。
- `strdup` 是保留启动写法的函数式宏。开启调试时返回可用 `free` 释放的空字符串；普通环境仍是标准的字符串复制。需要原生调用时写 `(strdup)(s)`，或在配置后 `#undef strdup`。
- 每次调用输出一行：行号、整组表达式、各参数值。表达式不拆分，因此比较符、模板参数、字符串中的逗号均不影响展示；参数各求值一次，求值顺序遵循 C++17 函数调用规则。
- 浮点用定点精度，布尔用 true/false；颜色和缩进由整组表达式稳定决定。先在局部缓冲中格式化，再一次写入 `std::cerr`，不修改其格式状态。
- 自定义 `operator<<` 优先，请自行保持单行输出。其中再次调用 `debug` 会被抑制；格式化异常继续向外传播，重入状态由 RAII 恢复。

## 支持边界

字符串和字符带引号并转义控制字符。`char[N]` 扫描不超出 N；`char*` 需为有效的零结尾字符串，空指针显示 nullptr。其他裸指针只打印地址，不解引用；查看指向值用 `debug(*p)`，并自行确保 p 有效。

容器按可迭代范围遍历，支持自定义比较器和分配器；顺序容器显示 `[]`，集合、映射显示 `{}`，pair/tuple 显示 `()`。空容器、空 tuple、optional、variant 可直接打印；unordered 容器的顺序遵循其迭代器。

同时公开 `key_type`、`mapped_type` 的可迭代容器按映射打印：用结构化绑定读取元素的键和值，再递归输出 `{(key, value), ...}`。`std::map`、`HashMap`、`TreeMap`、`TreeMapOff` 共用此路径，调试器不依赖具体 Map 类型，支持先后任意包含；自定义 `operator<<` 仍优先。TreeMap 按比较器顺序输出，value 按引用格式化；HashMap 按自身遍历顺序输出，value 遵循其迭代器的复制契约，打印要求 Val 可复制。普通 `vector<pair<...>>` 保持序列格式 `[(key, value), ...]`。

可默认初始化的普通聚合结构体最多展开 8 个直接字段，按引用读取，不复制字段；空结构体显示 `{}`。C++17 不具备完整反射：带基类、引用成员、位域或 C 数组成员的复杂聚合不在自动展开契约内，请提供 `operator<<`。union、超过 8 个字段、无法识别的类型显示 `<unprintable>`；不保证能自动识别所有复杂聚合并安全回退。

配置初始化须早于第一次 debug，使用单线程竞赛程序契约。重入保护按线程保存，但并发修改配置和跨线程原子输出不在保证内。宏关闭时参数不求值；开启后的嵌套 debug 参数仍会求值，但不产生嵌套输出。

输出耗时为遍历值与格式化的成本，辅助空间为这一行的字节数；不缓存表达式，不递归解引用指针。

## 正确性测试

具名用例位于 `tests/Correctness/Clarketech/DebugerHeadCopyToBitsStdC++/`，覆盖配置/格式化、通用映射协议、重复包含、多翻译单元及启动片段。用例已登记到 `Cases.json`；通过状态与工作量以当前测试报告为准。

覆盖范围：GCC/Clang 严格编译、多翻译单元与重复包含、空参数/空 tuple/空聚合、0–8 字段及不可复制字段、自定义 operator<<、复杂表达式、容器比较器、C 数组边界/空指针/转义、配置失败、精度与 cerr 状态、嵌套打印与异常恢复、宏关闭的副作用。启动集成逐字检查重复 `debug(x, y, z)` 的行号、名称、值与换行。此输出工具无需性能测试；对应性能目录保留 `Benchmark.cpp` 占位，`Settings.json` 声明跳过原因，报告显示“无需性能测试”。
