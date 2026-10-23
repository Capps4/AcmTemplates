# Debuger

[code.hpp](code.hpp)

竞赛单翻译单元使用以下接入片段：

```cpp
auto $ = strdup("color: false, space: false, precision: 6");
#ifndef COMPETITION_DEBUGER
    #define debug(...)
#endif
```

[长版调试器](../DebugerHeadCopyToBitsStdC++/README.md) 通过本地 `-include` 或标准头加载，并定义 `COMPETITION_DEBUGER`、`debug` 和配置入口；题解保留以上片段即可。普通环境中 `debug` 参数不求值。两种环境下 `$` 均可用 `free` 释放，配置只写在主程序翻译单元。
