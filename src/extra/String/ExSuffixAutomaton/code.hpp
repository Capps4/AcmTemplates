#pragma once
#include "../../../../Headers/Headers.hpp"

// 0 为根，根 link=-1；每个源串从根开始，避免跨串子串。
// 只维护结构；add(p,c) 的 p 是已有前缀节点，整串回调 fn(p, i)。
// 每个实际前缀都回调，包括复用节点；单纯克隆不登记输入贡献。
// 扩展次数 L、最大前缀长度 M：节点数 V<=2L+1，空间 O(DV)。
// 单次扩展最坏 O(M+D)；getOrder O(V+M)，回调成本另计。
// 外部数组按节点数扩展；追加后重新取得顺序和外部汇总。
template <int D = 26, char Base = 'a'>
class ExSam {
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

    explicit ExSam(int n = 0) {
        assert(n >= 0);
        std::size_t cap = 2 * std::size_t(n) + 1;
        son.reserve(cap);
        link.reserve(cap);
        len.reserve(cap);
        son.emplace_back();
        link.push_back(-1);
        len.push_back(0);
    }

    // 返回本次实际前缀的节点；可能复用或拆分已有状态。
    int add(int p, char c) {
        assert(0 <= p and p < int(len.size()));
        int v = code(c);
        if (son[p][v]) {
            int y = son[p][v];
            if (len[y] != len[p] + 1) {
                int cl = make(len[p] + 1, y);
                while (p != -1 and son[p][v] == y) {
                    son[p][v] = cl;
                    p = link[p];
                }
                link[y] = cl;
                y = cl;
            }
            return y;
        }
        int cur = make(len[p] + 1);
        while (p != -1 and not son[p][v]) {
            son[p][v] = cur;
            p = link[p];
        }
        if (p == -1)
            link[cur] = 0;
        else {
            int y = son[p][v];
            if (len[y] == len[p] + 1)
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
        return cur;
    }

    // 每串从根开始；回调下标从本串的 0 开始，闭包只借用不保存。
    template <class Func = void (*)(int, int)>
    int add(std::string_view s, Func &&fn = [](int, int) {}) {
        int p = 0;
        for (int i = 0; i < int(s.size()); ++i) {
            p = add(p, s[i]);
            fn(p, i);
        }
        return p;
    }

    std::vector<int> getOrder() const {
        int n = *std::max_element(len.begin(), len.end());
        std::vector<int> cnt(n + 1), ord(len.size());
        for (int d : len)
            ++cnt[d];
        for (int i = 1; i <= n; ++i)
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
