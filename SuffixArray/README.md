# SuffixArray / GNU++17

保留 n/sa/rk/h，空输入得到空数组、h[0]=0；修复旧版 n=0/1、越界 iota 与未检查长度的 LCP。byte 字母表初排用 256 项计数排序，按 unsigned 字节顺序；泛型值先按 comparator 压缩排名，支持负整数、极值和自定义记录。if constexpr 选择初排路径，后续倍增/计数排序维持 O(n log n)。排名更新改用临时数组交换，临时工作区在构造结束后释放。

sa[i] 是第 i 小后缀的起点，rk[pos] 是该后缀排名，h[i] 是 sa[i] 与 sa[i-1] 的 LCP（h[0]=0）。全部 0index，后缀覆盖 [pos,n)，长度 n<INT_MAX；构造读取有 size()/operator[] 的序列，之后不借用输入。C 字符串需 NUL 终止；有内嵌 NUL 请用 string/string_view。自定义 comparator 必须是严格弱序，等价的元素按 comparator 等价定义参与 LCP。默认 char/signed char/unsigned char 按 0..255 的字节顺序，自定义 comparator 可覆盖排序规则。

泛型初排 O(n log n)，byte 初排 O(n+256)，后续倍增 O(n log n)，LCP O(n)，总额外空间 O(n)，交付对象仅保留 3n 个 int（不含 vector 容量/对象开销）。类型大驼峰，变量/助手小驼峰；公开结果不能任意修改后仍假定相互一致。

测试：穷举 0..11 位二进制；朴素逐后缀排序和逐对 LCP oracle，1000 个随机字节/整数序列、负数与 LLONG_MIN/MAX、非整数记录和 move-only comparator、字面量、200000 同字符长输入。GCC -O2 与 Clang ASan/UBSan 均通过。

自评：没有改成更复杂的 SA-IS，保留倍增思想；边界检查消除对序列额外哨兵的依赖。131072 长 alphabet 1/2/26 构造约 1.40x/1.15x/1.12x。旧基准只使用旧版合法的非空 ASCII string，完整 sa/rk/h 校验和一致，数据见 Benchmark.csv。

源码、依赖和运行结果由统一入口记录于 Reports；目录中的 Results.json 为历史报告。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check SuffixArray`；性能用 `python3 template/Run.py bench SuffixArray`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
