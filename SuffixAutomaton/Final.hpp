#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <string>
#include <string_view>
#include <vector>

// SNIPPET BEGIN
template <int Z, char Base>
class Sam {
    static_assert(Z > 0 && int(static_cast<unsigned char>(Base)) + Z <= 256);

    static int code(char c) {
        int v = int(static_cast<unsigned char>(c)) - int(static_cast<unsigned char>(Base));
        assert(0 <= v && v < Z);
        return v;
    }

    int newNode(int d, int s = -1) {
        if (s < 0) {
            son.emplace_back();
            link.push_back(0);
        } else {
            son.push_back(son[s]);
            link.push_back(link[s]);
        }
        len.push_back(d);
        // cnt.push_back(0);
        return ++tot;
    }

public:
    std::vector<std::array<int, Z>> son{};
    std::vector<int> link{}, len{};
    // std::vector<long long> cnt{}; // 统计出现次数时启用相关注释，再沿 link 累计。
    int last = 0, tot = 0;

    explicit Sam(int n = 0) {
        int capacity = 2 * n + 1;
        son.reserve(capacity);
        link.reserve(capacity);
        len.reserve(capacity);
        son.emplace_back();
        link.push_back(-1);
        len.push_back(0);
        // cnt.push_back(0);
    }

    explicit Sam(std::string_view s) : Sam(int(s.size())) {
        for (char c : s)
            add(c);
    }

    int add(char c) {
        int v = code(c);
        int cur = newNode(len[last] + 1), p = last;
        // cnt[cur] = 1;
        while (p != -1 && !son[p][v]) {
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
                int clone = newNode(len[p] + 1, y);
                while (p != -1 && son[p][v] == y) {
                    son[p][v] = clone;
                    p = link[p];
                }
                link[y] = link[cur] = clone;
            }
        }
        return last = cur;
    }

    int add(int id, char c) {
        (void)id;
        return add(c);
    }

    bool contains(std::string_view s) const {
        int p = 0;
        for (char c : s) {
            p = son[p][code(c)];
            if (!p) return false;
        }
        return true;
    }

    long long distinctSubstringCount() const {
        long long res = 0;
        for (int p = 1; p <= tot; ++p)
            res += len[p] - len[link[p]];
        return res;
    }
};
