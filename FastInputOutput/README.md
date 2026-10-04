# 缓冲读写（GNU++17）

保留 Original 1MiB 缓冲区、输入 sentinel 和双位整数读写。`QInput(FILE*=stdin)`、`QOutput(FILE*=stdout)`；旧 Qinput/Qoutput 是别名，`fastIn()/fastOut()` 是共享流。删除全局 cin/cout 宏，避免污染别的模块。对象不可复制，借用 FILE，输出对象析构时 flush。

支持整数（含 128 位）、bool/short、字符/字符串、浮点、string_view 输出和常用 endl/flush/ends；保留 fail/eof/clear/getLine 及浮点精度输出。整数输入必须落在目标类型范围；没有 readChecked 扩展或 Checked 模板。非法 token/IO 失败有失败状态，输入 EOF 不覆盖原值。

本轮以 Original 为基准：保留 Original 缓冲区、sentinel 和双位读写内核。删除 Checked 模板/readChecked 数值范围框架；保留常用标量/浮点/视图和显式流，避免全局 cin/cout 宏。

正确性测试、ASan/UBSan 及旧版/新版性能证据见 Results.json、Benchmark.csv 和 [Performance.md](Performance.md)。倍率只适用于本机和对应输入。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check FastInputOutput`；性能用 `python3 template/Run.py bench FastInputOutput`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
