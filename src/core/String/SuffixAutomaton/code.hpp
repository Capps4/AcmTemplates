#pragma once
#include "../../../../Headers/Headers.hpp"

// 0 为根，根 link=-1；缺边为 0，连续字节字母表。
// 只维护结构；每次字符扩展后回调 fn(p, i)，克隆不调用，外部数组按节点数扩展。
// V<=2n+1：构建 O(Dn)，getOrder O(n+V)，空间 O(DV)，回调成本另计。
// 整串下标从本次字符串的 0 开始；追加后重新取得顺序和外部汇总。
template <int D = 26, char Base = 'a'>
class Sam {
    static_assert(D > 0 and int(static_cast<unsigned char>(Base)) + D <= 256);

    static int code(char c) {
        int v = int(static_cast<unsigned char>(c)) - int(static_cast<unsigned char>(Base));
        assert(0 <= v and v < D);
        return v;
    }

    int make(int d, int s = -1) {
        if (s < 0) {
            son.emplace_back();
            link.push_back(0);
        } else {
            son.push_back(son[s]);
            link.push_back(link[s]);
        }
        len.push_back(d);
        return int(len.size()) - 1;
    }

public:
    std::vector<std::array<int, D>> son{};
    std::vector<int> link{}, len{};
    int last = 0;

    explicit Sam(int n = 0) {
        assert(n >= 0);
        std::size_t cap = 2 * std::size_t(n) + 1;
        son.reserve(cap);
        link.reserve(cap);
        len.reserve(cap);
        son.emplace_back();
        link.push_back(-1);
        len.push_back(0);
    }

    // 返回新前缀节点；克隆只复制 son/link 并设置 len。
    int add(char c) {
        int v = code(c);
        int cur = make(len[last] + 1), p = last;
        while (p != -1 and not son[p][v]) {
            son[p][v] = cur;
            p = link[p];
        }
        if (p == -1)
            link[cur] = 0;
        else {
            int y = son[p][v];
            if (len[p] + 1 == len[y])
                link[cur] = y;
            else {
                int cl = make(len[p] + 1, y);
                while (p != -1 and son[p][v] == y) {
                    son[p][v] = cl;
                    p = link[p];
                }
                link[y] = link[cur] = cl;
            }
        }
        last = cur;
        return cur;
    }

    // 结构就绪（包括克隆）后再回调；空串不调用，闭包只借用不保存。
    template <class Func = void (*)(int, int)>
    void add(std::string_view s, Func &&fn = [](int, int) {}) {
        for (int i = 0; i < int(s.size()); ++i) {
            int p = add(s[i]);
            fn(p, i);
        }
    }

    std::vector<int> getOrder() const {
        std::vector<int> cnt(len[last] + 1), ord(len.size());
        for (int d : len)
            ++cnt[d];
        for (int i = 1; i < int(cnt.size()); ++i)
            cnt[i] += cnt[i - 1];
        for (int p = int(len.size()) - 1; p >= 0; --p)
            ord[--cnt[len[p]]] = p;
        return ord;
    }

    // 缺失为 nullopt，空串返回有值的根 0；非空长度位于 (len[link[p]], len[p]]。
    std::optional<int> find(std::string_view s) const {
        int p = 0;
        for (char c : s) {
            p = son[p][code(c)];
            if (not p)
                return std::nullopt;
        }
        return p;
    }

    // 固定自动机上流式匹配；初始 p=l=0，随后保持 len[link[p]]<l<=len[p]。
    void match(int &p, int &l, char c) const {
        assert(0 <= p and p < int(len.size()) and 0 <= l and l <= len[p]);
        int v = code(c);
        while (p and not son[p][v]) {
            p = link[p];
            l = std::min(l, len[p]);
        }
        if (son[p][v]) {
            p = son[p][v];
            ++l;
        } else {
            p = 0;
            l = 0;
        }
    }
};
