# Original 风格重塑完成

51/51 个模块已对照 Original 重新审查、整理并验证。Original.hpp 与 Original.code-snippets 共 102 份快照保持原样。改动前版本及报告保存在 VerificationHistory/BeforeContestReshape.tar.gz，各模块的保留与简化原因见 StyleReview.json。

保留 Original 的算法、字段及函数名称；新增临时变量尽量使用清楚的 1–4 字符名字。短返回和短判断可以单行，多步逻辑展开。恢复 DSU 原合并方向、主席树递归查询、RMQ 按值返回和最小轮转管道，删除重复接口、额外模板分支、极大容量检查及不用的常驻数据。用户已经确认的 ListHelper sorted、HashMap 简洁接口保留。

正确性修复继续保留：空输入、任意字节、宽化前溢出、环树末边、重复求解、深链显式栈等；MaxFlow 按用户要求恢复 Original 递归，不再维护 Frame/stk。筛法的整数重载保留一行约束，因为删除后 Clang 会出现重载歧义。多 case 入口先过滤非正数，再使用 Original 的 while(cases--)，避免最小负数递减溢出。

全部模块通过 GNU++17 GCC -O2 和 Clang ASan/UBSan；消毒器设置 halt_on_error，首次报错即停止。50 个模块的 52 个独立消毒器程序另做复查；多 case 入口由专门输入用例及真实片段联合测试覆盖。49 个实际库片段、最终头文件和两个展开后的入口均通过联合测试，包括原样 Geo2/Geo3 接入。

所有模块的 Original/Final 七轮中位数评测和校验值已更新。另对最小轮转、主席树、RMQ、Fenwick 做本轮修改前 Final/修改后 Final 的直接对照，最终计时串行执行，保留原始样本。详见 VerificationHistory/ContestReshapeBench/README.md；简化不意味着所有场景都提速，例如主席树 kth 本次慢约 5%，深链安全也有常数开销。

51 个安装用片段已重新导出到 ReadyToInstall，并核对源码、依赖、报告及片段一致性。VS Code 安装目录没有被写入。Geo2/Geo3 不升级；快照仅用于兼容验证。

## 严格编译检查完成

按用户参数逐条验证 49 个模块（暂缓几何和持久化树）：-std=gnu++17 -Wall -Wextra -Werror -Weffc++ -O0 -g -rdynamic -fno-omit-frame-pointer。实例化测试、独立用例、实际库片段及两个入口均通过。容器成员补 {}，普通索引/度数/标记恢复 int/char，ExGcd 恢复 Original 四参递归与 constexpr；没有 Result。原注释中的九组可选功能也启用后验证。检查过程与复跑命令见 Strict/README.md。
