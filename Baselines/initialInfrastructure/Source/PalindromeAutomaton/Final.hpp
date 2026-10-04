#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <string>
#include <string_view>
#include <vector>

// SNIPPET BEGIN
template <int Z, char Base>
class Pam {
    static_assert(Z > 0 && int(static_cast<unsigned char>(Base)) + Z <= 256);

    static int code(char c) {
        int v = int(static_cast<unsigned char>(c)) - int(static_cast<unsigned char>(Base));
        assert(0 <= v && v < Z);
        return v;
    }

public:
    std::vector<std::array<int, Z>> son{};
    std::vector<int> link{}, len{}, dep{}, cnt{};
    std::string s{};
    int cur = 0, tot = 1;

    explicit Pam(int n = 0) {
        int capacity = n + 2;
        son.reserve(capacity);
        link.reserve(capacity);
        len.reserve(capacity);
        dep.reserve(capacity);
        cnt.reserve(capacity);
        s.reserve(n);
        son.resize(2);
        link = {1, 1};
        len = {0, -1};
        dep.resize(2);
        cnt.resize(2);
    }

    explicit Pam(std::string_view s) : Pam(int(s.size())) {
        for (char c : s)
            add(c);
    }

    int getLink(int p, int i) const {
        assert(0 <= p && p <= tot && 0 <= i && i < int(s.size()));
        while (i - len[p] - 1 < 0 || s[i - len[p] - 1] != s[i])
            p = link[p];
        return p;
    }

    int add(char c) {
        int v = code(c);
        int i = int(s.size());
        s.push_back(c);
        int p = getLink(cur, i);
        if (!son[p][v]) {
            int f = son[getLink(link[p], i)][v];
            son[p][v] = ++tot;
            son.emplace_back();
            link.push_back(f);
            len.push_back(len[p] + 2);
            dep.push_back(dep[f] + 1);
            cnt.push_back(0);
        }
        cur = son[p][v];
        ++cnt[cur];
        return cur;
    }

    int add(int i, char c) {
        assert(i == int(s.size()));
        return add(c);
    }

    std::vector<long long> occurrences() const {
        std::vector<long long> res(cnt.begin(), cnt.end());
        for (int p = tot; p >= 2; --p)
            res[link[p]] += res[p];
        return res;
    }

    std::vector<std::vector<int>> getLinkTree() const {
        // Pam 的 linkTree 以 1 为根。
        std::vector<std::vector<int>> g(tot + 1);
        for (int p = 0; p <= tot; ++p)
            if (p != 1) g[link[p]].push_back(p);
        return g;
    }
};
