# FastInputOutput

[code.hpp](code.hpp)

`Qinput/Qoutput` 使用 1 MiB 缓冲区，支持整型（含 GNU 128 位）、字符、string、`getLine` 及 `endl/flush/ends`。读入整数须能放进目标类型，不提供浮点输出适配。

```cpp
#define cin ref(Qinput::shared()).get()
#define cout ref(Qoutput::shared()).get()
```

本头最后 include；直接 `cin >> x; cout << x;`。宏同样会展开 `std::cin/std::cout` 与 `seq::cin/seq::cout`，其他流管道应先建立。

对象借用 FILE，不可复制；析构刷新输出，提供 `fail/eof/clear`。读写成本与字符数量成正比。
