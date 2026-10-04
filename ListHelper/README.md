# ListHelper

原版已大量使用 C++14 泛型 lambda、移动捕获和 C++17 if constexpr；保留基数排序、排序快路径和立即执行管道；合入当前原文件已有的降序/11 位分桶/相同桶轮次跳过改进。此次重点是减少重复复制、补全所有权与调用约定。

缓存的 `Op` 在非 const 左值/右值调用时直接执行捕获对象，支持不可复制的回调；可变回调状态会跨调用保留。const 操作能 const 调用则直接执行，否则复制可复制回调后调用。累计初值可复制时每次复制，非复制初值会被消费。`first` 对右值容器移动匹配元素，对左值复制匹配元素。

```cpp
std::vector<int> a{3, 1, 3};
auto b = a | sorted() | unique();
auto c = std::string_view("banana") | slice(1, 4); // owning string
std::istringstream input("4 5");
auto d = std::vector<int>(2) | readFrom(input);
b | writeTo(std::cout);
```

转换操作支持 vector/string，string_view 转换返回 string；切片使用 0index `[first,last)`，界限裁剪到合法范围，反向界限得到空结果。sorted/filter 等并非所有标准容器通用（需要相应 iterator/erase/reserve/allocator 接口）。`vector<bool>` map 输入按 bool 值读取，避免捕获临时代理。read 遇失败停止，失败位置遵循流本身的提取语义，不承诺保留该位置旧值。

`readFrom`/`writeTo` 接受具体流类型，保留返回类型与自定义流能力。旧 `seq::cin`/`seq::cout` 包装仍可用；不再全局导入这两个名称。`MemberCall` 用 CTAD、std::apply/std::invoke 执行；lvalue 接收者 const 借用、rvalue 消费，临时对象的引用结果转为拥有的值，void 调用返回接收者。传入的 lvalue 参数仍被借用。返回 pointer/view/iterator 或回调返回的其他借用结果，仍需调用者管理寿命；不能把所有返回类型都当作拥有结果。`call` 宏的可选逗号使用 GNU 扩展。

测试包含随机排序、符号极值、不同 radix 位宽、缓存有状态/不可复制回调、重复累计、bool 代理、string_view 全切片边界、移动 unique_ptr、流失败以及成员函数/成员指针调用。

风格自审：类型大驼峰，方法/变量小驼峰；保留 seq 命名空间和现有 call 宏。没有引入延迟视图链，结果寿命由上述接口明确约定。当前性能数据见 [Performance.md](Performance.md)：基数排序内核沿用当前源文件的策略，重捕获缓存场景测量的是复制消除收益。

早先基准显示普通切片有回退，缓存重捕获有明显收益；当前基准已切换到更新后的源文件，具体数值以 Performance.md 为准。

源文件核对发现 ListHelper 的排序部分已在初始快照之后发生变更；不推断变更来源，也未写入原目录。初始 Original 快照保留，当前源文件另存 CurrentSource.code-snippets/CurrentSource.hpp，其 SHA256 在 Manifest.json。此次最终版合入该排序改进并保留自己的所有权/捕获修复；Benchmark.cpp 使用 CurrentSource.hpp 对照。

基数排序支持随机访问的整数 vector/deque/string（不含 bool）；标准 less<T>/less<>/greater<T>/greater<> 自动识别升降序。自适应 8/11 位策略在大输入减少轮次，单桶轮次直接跳过。直接 radixSort<B,Descending> 要求给出有效 min/max 界限；空或全相等输入直接返回，B 在 1..16。字符排序遵循对应 char 类型的数值顺序，与 minimalRotationIndex 默认无符号字节顺序区分。新增测试覆盖 typed comparator、降序、deque、全部字符及中间多轮单桶跳过。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check ListHelper`；性能用 `python3 template/Run.py bench ListHelper`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
