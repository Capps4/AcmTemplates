#pragma once
#include "../../../../Headers/Headers.hpp"

// 0 是空回文根，1 是长度 -1 的虚根，真实回文编号从 2 开始。
// 只维护结构；add 返回最长回文后缀，每次字符扩展后回调 fn(p, i)。
// link 指向更早创建的节点；两个根的汇总不代表空串次数，link 树根为 1。
// 长度 n：小字母表构建 O(Dn)，空间 O(DV+n)，回调成本另计。
template <int D = 26, char Base = 'a'>
class Pam {
    static_assert(D > 0 and int(static_cast<unsigned char>(Base)) + D <= 256);

    static int code(char c) {
        int v = int(static_cast<unsigned char>(c)) - int(static_cast<unsigned char>(Base));
        assert(0 <= v and v < D);
        return v;
    }

    int getLink(int p, int i) const {
        while (i - len[p] - 1 < 0 or s[i - len[p] - 1] != s[i])
            p = link[p];
        return p;
    }

public:
    std::vector<std::array<int, D>> son{};
    std::vector<int> link{}, len{};
    std::string s{};
    int cur = 0;

    explicit Pam(int n = 0) {
        assert(n >= 0);
        std::size_t cap = std::size_t(n) + 2;
        son.reserve(cap);
        link.reserve(cap);
        len.reserve(cap);
        s.reserve(n);
        son.resize(2);
        link = {1, 1};
        len = {0, -1};
    }

    // 返回当前最长回文后缀；单字符贡献通过返回值在外部登记。
    int add(char c) {
        int v = code(c);
        int i = int(s.size());
        s.push_back(c);
        int p = getLink(cur, i);
        if (not son[p][v]) {
            int f = son[getLink(link[p], i)][v];
            son[p][v] = int(son.size());
            son.emplace_back();
            link.push_back(f);
            len.push_back(len[p] + 2);
        }
        cur = son[p][v];
        return cur;
    }

    // 回调下标从本次字符串的 0 开始；text 不能借用本对象的 s。
    template <class Func = void (*)(int, int)>
    void add(std::string_view text, Func &&fn = [](int, int) {}) {
        for (int i = 0; i < int(text.size()); ++i) {
            int p = add(text[i]);
            fn(p, i);
        }
    }
};
