# MultipleBackpacks

保留降序余数链上的单调队列算法，使用 C++17 结构化绑定读取 {value,weight,count}。跳过零数量、非正价值及超容量物品；正价值零体积物品直接增加所有容量的价值。修正原版零体积遗漏和 up+weight 可能溢出的界限写法。

```cpp
auto best = multiBag(VecGood{{3, 0, 4}, {5, 2, 3}}, 5);
// best: 12,12,17,17,22,22
```

best[c] 表示总重量至多 c 的最大价值，不要求恰好装满。容量/重量/数量非负，capacity<INT_MAX，所有累计价值需在 long long 范围内；负价值物品可舍弃。输入和容量 0index，DP 数组范围 `[0,capacity+1)`。数量裁剪到 capacity/weight，避免无效窗口范围。有效正重量物品每种 O(capacity)，零重量物品 O(capacity)，超容量/无价值物品 O(1)，额外空间 O(capacity)。

测试三千随机小问题，与枚举每种选取数量的朴素 DP 对照，覆盖零体积/容量/数量、负价值和 INT_MAX 权重/数量边界。旧版不支持零体积，基准只比较合法正重量输入；超容量场景仍是合法输入，体现避免扫描空余数链的收益。当前性能见 [Performance.md](Performance.md)。

风格自审：保留 VecGood、multiBag 名称，类型大驼峰，局部小驼峰；先处理边界再进入内核。沿用原版降序更新以避免每种复制整个 DP，没有添加额外物品类型或策略层。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check MultipleBackpacks`；性能用 `python3 template/Run.py bench MultipleBackpacks`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
