#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <initializer_list>
#include <set>
#include <string>
#include <string_view>
#include <vector>

// SNIPPET BEGIN
template <int Z, char Base>
class ExSam {
    static_assert(Z > 0 && int(static_cast<unsigned char>(Base)) + Z <= 256);

    static int code(char c) {
        int v = int(static_cast<unsigned char>(c)) - int(static_cast<unsigned char>(Base));
        assert(0 <= v && v < Z);
        return v;
    }

    [[gnu::always_inline]] int newNode(int d, int s = -1) {
        if (s < 0) {
            son.emplace_back();
            link.push_back(0);
        } else {
            son.push_back(son[s]);
            link.push_back(link[s]);
        }
        len.push_back(d);
        // ID.emplace_back();
        return ++tot;
    }

public:
    std::vector<std::array<int, Z>> son{};
    std::vector<int> len{}, link{};
    // std::vector<std::set<int>> ID{}; // 状态关联的字符串编号；启用相关注释。
    int tot = 0, last = 0;

    ExSam() {
        son.emplace_back();
        link.push_back(-1);
        len.push_back(0);
        // ID.emplace_back();
    }

    ExSam(std::initializer_list<std::string_view> strings)
        : ExSam(std::vector<std::string_view>(strings)) {}

    template <class Range>
    explicit ExSam(const Range &strings) : ExSam() {
        int sum = 0;
        for (const auto &s : strings) {
            sum += s.size();
        }
        son.reserve(2 * sum + 1);
        link.reserve(2 * sum + 1);
        len.reserve(2 * sum + 1);
        int id = 0;
        for (const auto &s : strings)
            addString(std::string_view(s), id++);
    }

    void addString(std::string_view s, int id = 0) {
        last = 0;
        for (char c : s)
            exadd(c, id);
    }

    int exadd(char c, int id = 0) {
        (void)id;
        int v = code(c), p = last;
        if (son[p][v]) {
            int y = son[p][v];
            if (len[y] != len[p] + 1) {
                int cl = newNode(len[p] + 1, y);
                while (p != -1 && son[p][v] == y) {
                    son[p][v] = cl;
                    p = link[p];
                }
                link[y] = cl;
                y = cl;
            }
            // ID[y].insert(id);
            return last = y;
        }
        int cur = newNode(len[last] + 1);
        while (p != -1 && !son[p][v]) {
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
                int cl = newNode(len[p] + 1, y);
                while (p != -1 && son[p][v] == y) {
                    son[p][v] = cl;
                    p = link[p];
                }
                link[y] = link[cur] = cl;
            }
        }
        // ID[cur].insert(id);
        return last = cur;
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
