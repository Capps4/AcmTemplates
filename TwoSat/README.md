# TwoSat

依赖 StronglyConnectedComponent。保留 implication/assign/work 接口与文字编码 `2*x+value`，使用显式栈 Scc 消除深递归；work 只请求组件标号，不构建未使用的凝聚图。

```cpp
TwoSat sat(3);
sat.add(0, true, 1, false); // x0=true ⇒ x1=false，自动加逆否命题
sat.assign(0, true);
if (sat.work()) { /* sat.ans */ }
```

变量为 0index `[0,n)`。add 表示蕴含，可将 `(x=f) or (y=g)` 写作 `add(x,!f,y,g)`。work 返回 false 时清空 ans，避免残留上次或部分答案；true 时 ans 长度 n。可以重复 work，或增加约束后再 work；空问题可满足。顶点/边均合法，2*n 须可表示为 int，构造与添加操作断言检查。复杂度 O(n+m)，其中 m 为加入的蕴含边数。

测试对两千五百组 n<=8 随机问题枚举全部布尔赋值，独立判断可满足性并验证输出解；覆盖重复求解、由可满足变为不可满足，以及二十万变量的强制链。性能见 [Performance.md](Performance.md)：原版和最终版采用同一访问顺序并比较解的校验和。

风格自审：TwoSat 大驼峰，小驼峰方法；保留既有数学接口，不添加重复的图封装。先检查所有矛盾，再发布完整答案，失败时明确清空。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check TwoSat`；性能用 `python3 template/Run.py bench TwoSat`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
