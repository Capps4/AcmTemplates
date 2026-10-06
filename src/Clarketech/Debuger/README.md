# Debuger

[code.hpp](code.hpp)

竞赛单翻译单元使用以下接入片段：

```cpp
auto $ = strdup("color: false, space: false, precision: 6");
#ifndef COMPETITION_DEBUGER
    #define debug(...)
#endif
```

外部调试器定义 `COMPETITION_DEBUGER` 时提供 `debug`；普通环境中参数不求值。`strdup` 返回的字符串可用 `free` 释放。
