#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

// SNIPPET BEGIN
template <int Z, char Base>
class AcAutomaton {
    static_assert(Z > 0 && int(static_cast<unsigned char>(Base)) + Z <= 256);

    static int code(char c) {
        int v = int(static_cast<unsigned char>(c)) - int(static_cast<unsigned char>(Base));
        assert(0 <= v && v < Z);
        return v;
    }

    int insert(std::string_view s) {
        int p = 0;
        for (char c : s) {
            int v = code(c);
            if (!son[p][v]) {
                son[p][v] = ++tot;
                // Initialize a block at a time; avoid touching the unused part
                // of the reserved worst-case table for duplicate patterns.
                if (tot == int(son.size())) {
                    auto size = std::min(son.capacity(), son.size() + 1024);
                    son.resize(size);
                    link.resize(size);
                }
            }
            p = son[p][v];
        }
        return p;
    }

    void build() {
        order.reserve(son.size());
        order.push_back(0);
        for (int y : son[0])
            if (y) order.push_back(y);
        for (int i = 1; i < int(order.size()); ++i) {
            int p = order[i];
            for (int v = 0; v < Z; ++v) {
                int y = son[p][v];
                if (y) {
                    link[y] = son[link[p]][v];
                    order.push_back(y);
                } else
                    son[p][v] = son[link[p]][v];
            }
        }
    }

public:
    std::vector<std::array<int, Z>> son{};
    std::vector<int> link{}, terminal{}, order{};
    // std::vector<std::vector<int>> ID{}; // 各终止节点的模式编号。
    int tot = 0;

    AcAutomaton(std::initializer_list<std::string_view> patterns)
        : AcAutomaton(std::vector<std::string_view>(patterns)) {}

    template <class Range>
    explicit AcAutomaton(const Range &patterns) {
        int sum = 0;
        for (const auto &s : patterns) {
            sum += s.size();
        }
        son.reserve(sum + 1);
        link.reserve(sum + 1);
        son.emplace_back();
        link.push_back(0);
        for (const auto &s : patterns)
            terminal.push_back(insert(std::string_view(s)));
        son.resize(tot + 1);
        link.resize(tot + 1);
        build();
        // 如需 ID，启用上面的成员及下面三行。
        // ID.resize(tot + 1);
        // for (int i = 0; i < int(terminal.size()); ++i)
        //     ID[terminal[i]].push_back(i);
    }

    std::vector<long long> countOccurrences(std::string_view s) const {
        std::vector<long long> cnt(son.size());
        cnt[0] = 1; // Empty patterns match every boundary, including the first.
        int p = 0;
        for (char c : s) {
            p = son[p][code(c)];
            ++cnt[p];
        }
        for (int i = int(order.size()); i > 1; --i) {
            int node = order[i - 1];
            cnt[link[node]] += cnt[node];
        }
        std::vector<long long> res;
        res.reserve(terminal.size());
        for (int node : terminal)
            res.push_back(cnt[node]);
        return res;
    }
};
