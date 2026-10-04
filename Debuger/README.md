# Debuger

这是外部调试器的配置/宏接入片段，不扩展成日志框架。保留 COMPETITION_DEBUGER 分支、debug(...) 宏和 GNU 标识符 `$`，配置文字完全一致。你的本机 bits/stdc++.h 将 strdup 宏替换为配置注册函数，最终版在该宏存在时照常调用；普通评测环境不分配内存，直接使用静态字符数组。C++17 inline 变量在一致编译配置下共享同一份配置。`$` 改为 const char* 借用指针，不应 free；需要改字串时通过 debugerOptions（仅修改数组不足以让外部调试器重新注册）。

禁用 debug 时参数不会求值；启用时不覆盖外部已定义宏。实际双翻译单元验证默认配置共享地址/写入和宏分支；独立 TestCasePlugin.cpp 在 GCC 下接入你本机的真实调试器，Clang 下用注册函数 fixture 验证相同宏契约。外部调试器自身头文件有非 inline 全局定义，仍按其单翻译单元竞赛用途使用；本次不改系统头文件。没有索引或区间 API；保留已有接口名称，包括约定的 `$` 和拼写 Debuger，配置存储变量小驼峰。

Benchmark 对照原配置字面量与最终配置数组的反复读取，不能量化调试打印或注册耗时；本机旧版通过宏注册，本就不执行 strdup 分配，注册返回空字串（最终版保留该返回值）。普通无插件环境才省一次分配。这里只宣称明确接入和简化所有权，不宣称读取加速。计时见 Performance.md，七轮原始数据见 Results.json。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check Debuger`；性能用 `python3 template/Run.py bench Debuger`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
