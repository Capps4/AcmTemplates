# Manacher / GNU++17

接受 string_view，构造期间读取文本、之后只保存回文长度数组；可以直接用子串视图或临时 string。移除旧版 `[`、`]`、`-` 哨兵与转化字符串，分别计算奇/偶长度回文的半径，再从右到左得到每个位置结尾的最长回文长度。输入可包含任意字节和 NUL，不依赖字符集排除规则。

保留 getPalinLenFromCenter(i,between) 与 getPalinLenFromTail(i)，返回完整长度。between=false 的中心为 i，between=true 为 i+0.5，兼容旧调用参数 0/1。新增 isPalindrome(l,r) 在 O(1) 判断 [l,r)，空区间返回 true。文本长 n<=INT_MAX，中心/结尾下标从 0 开始；半字符中心 i 要求 i<n-1，普通中心/结尾要求 i<n。文本为空时仅空区间查询有效。断言关闭后仍需满足这些边界。

旧版尾长来自转化串半径传播；新版从最长奇/偶回文的右端点记录长度，向前每次减 2（两端各删一个字符），保持返回意义。三个 n 项 int 数组取代两个约 2n 项数组，同时省去约 2n 字符转化串。O(n) 构建，O(n) 空间，所有查询 O(1)。

测试：穷举长度 0..11 的所有二进制字符串；逐中心、逐尾位置及所有 [l,r) 区间，与直接逐字符回文检测比对。覆盖旧哨兵字符、完整 256 字节集、1000 个随机字节串及其子视图、临时 string 与百万重复字符。GCC -O2、Clang ASan/UBSan 均通过。

性能：百万字节 alphabet=1/2/26，含构建与全部中心/尾查询，约 2.23x/1.24x/2.88x；子串视图构造约 2.14x。Benchmark.csv 是本机七轮中位数，校验和相同；基准输入只用旧版合法的 a..z，含哨兵/NUL 的输入仅检查新版正确性。没有用旧版越界输入证明性能。

自评：类型 Manacher 大驼峰，方法/变量 lowerCamelCase，0index、[l,r) 明确。奇偶数组表示属于算法修正，string_view 属于 C++17 的接口改进；不能把整个加速都归于换编译标准。输入在构造后不借用，源码/snippet 匹配，SHA256 与验证报告当前有效。原 VS Code 文件未覆盖。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

本轮以 Original 为基准：保留已验证且较快的 odd/even 内核：Original 字符哨兵与越界不安全。去掉 checkedSize，保留短回文区间查询，不增加对象 metadata。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check Manacher`；性能用 `python3 template/Run.py bench Manacher`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
