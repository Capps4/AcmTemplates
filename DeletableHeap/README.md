# DeletableHeap GNU++17

DeletableHeap<T,Cmp> 保留 push/erase/top/pop/size，新增 empty、emplace、传入比较器和 vector 批量建堆（支持 CTAD）。Cmp 必须可复制，排序规则应固定。erase(x) 的前置条件是当前存在一个 x；按一次 erase 删除一次出现。比较器等价关系必须与 == 一致，否则惰性删除无法匹配堆顶。

top 返回 const T&，省去结果复制；任何修改/同步操作后不继续使用旧引用。push/erase 移动传入值，emplace 转发构造参数。vector 构造走标准库 O(n) heapify，避免逐次 O(log n) push。push/erase O(log n)，top/pop 的同步可能清理多个历史标记，按每个删除项最多清理一次计摊还成本。内存取决于尚未弹出的历史项，erase 不保证立即释放全部空间。

Test.cpp 对比 multiset 的 5 万随机操作，覆盖重复值、批量建堆、带捕获比较器、string emplace 和借用 top 引用。Benchmark.cpp 比较混合 push/erase/top 和 3 万字符串批量构造；GCC -O2、Clang ASan/UBSan、七轮中位数记录见 Results.json。

自我 Review：类型大驼峰，方法/变量小驼峰；无区间接口，0index/[l,r) 不适用。保留双堆惰性删除职责，不额外建立计数表验证 erase，以免所有操作承担额外哈希成本。接口前置条件明确，不把无法验证的删除合法性当成有运行时检查。

当前计时方法及数据见 [Performance.md](Performance.md)。本文已有数字属于此前计时记录，原报告保存在 `../VerificationHistory/BeforeTimingFence/`；以 Performance.md 为当前性能依据。

## 当前验证入口

工程根目录执行 `python3 template/Run.py check DeletableHeap`；性能用 `python3 template/Run.py bench DeletableHeap`。
当前结果见 Reports；本目录原 Results.json / Performance.md 中的数据是历史记录。流程说明见 [主 README](../README.md)。
