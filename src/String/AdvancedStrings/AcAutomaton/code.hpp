#pragma once
#include "../../../../Headers/Headers.hpp"

// 0 为根，son 缺边为 0。连续字节字母表，节点编号用 int。
// add 只构建 Trie；build 补全转移，此后不能插入。重复 build 无副作用。
// 只维护结构；每次字符扩展后回调 fn(p, i)，i 从本串的 0 开始。
// V 为节点数：build O(DV)，step O(1)，空间 O(DV)，回调成本另计。
template <int D = 26, char Base = 'a'>
class AcAutomaton {
    static_assert(D > 0 and int(static_cast<unsigned char>(Base)) + D <= 256);
    bool done = false;

    static int code(char c) {
        int v = int(static_cast<unsigned char>(c)) - int(static_cast<unsigned char>(Base));
        assert(0 <= v and v < D);
        return v;
    }

public:
    std::vector<std::array<int, D>> son{};
    std::vector<int> link{}, order{};

    explicit AcAutomaton(int n = 0) {
        assert(n >= 0);
        std::size_t cap = std::size_t(n) + 1;
        son.reserve(cap);
        link.reserve(cap);
        son.emplace_back();
        link.push_back(0);
    }

    // 返回 Trie 子节点，只维护结构，不登记完整模式的贡献。
    int add(int p, char c) {
        assert(not done and 0 <= p and p < int(son.size()));
        int v = code(c);
        if (not son[p][v]) {
            int y = int(son.size());
            son.emplace_back();
            link.push_back(0);
            son[p][v] = y;
        }
        return son[p][v];
    }

    // 复用节点也调用 fn，空串不调用；闭包仅在本次调用期间借用。
    template <class Func = void (*)(int, int)>
    int add(std::string_view s, Func &&fn = [](int, int) {}) {
        assert(not done);
        int p = 0;
        for (int i = 0; i < int(s.size()); ++i) {
            p = add(p, s[i]);
            fn(p, i);
        }
        return p;
    }

    void build() {
        if (done)
            return;
        order.push_back(0);
        for (int y : son[0])
            if (y)
                order.push_back(y);
        for (int i = 1; i < int(order.size()); ++i) {
            int p = order[i];
            for (int v = 0; v < D; ++v) {
                int y = son[p][v];
                if (y) {
                    link[y] = son[link[p]][v];
                    order.push_back(y);
                } else
                    son[p][v] = son[link[p]][v];
            }
        }
        done = true;
    }

    int step(int p, char c) const {
        assert(done and 0 <= p and p < int(son.size()));
        return son[p][code(c)];
    }
};
