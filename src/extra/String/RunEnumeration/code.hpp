#pragma once
#include "Include.hpp"

inline std::vector<std::tuple<int, int, int>> runEnumeration(std::string_view s) {
    using u64 = unsigned long long;
    assert(s.size() <= std::size_t(INT_MAX / 2));
    int n = s.size();
    // Every maximal repetition has this same interval at its least period.
    std::unordered_map<u64, int> tab;
    auto put = [&](int p, int l, int r) {
        if (r - l < 2 * p)
            return;
        if (l > 0 and s[l - 1] == s[l - 1 + p])
            return;
        if (r < n and s[r] == s[r - p])
            return;
        u64 key = (u64(l) << 32) | unsigned(r);
        auto [it, ins] = tab.emplace(key, p);
        if (not ins)
            it->second = std::min(it->second, p);
    };
    auto calc = [&](const std::string &a, const std::string &b, int m, bool rev) {
        int l = a.size(), r = b.size();
        std::string t(a.rbegin(), a.rend());
        auto x = zFunction(t);
        t = b + a + b;
        auto y = zFunction(t);
        for (int p = 1; p <= l; ++p) {
            int u = p == l ? l : std::min(p + x[p], l);
            int v = std::min(y[l + r - p], r);
            if (rev)
                put(p, m - v, m + u);
            else
                put(p, m - u, m + v);
        }
    };
    auto dfs = [&](auto &self, int l, int r) -> void {
        if (r - l <= 1)
            return;
        int m = l + (r - l) / 2;
        self(self, l, m);
        self(self, m, r);
        std::string a(s.substr(l, m - l)), b(s.substr(m, r - m));
        calc(a, b, m, false);
        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());
        calc(b, a, m, true);
    };
    dfs(dfs, 0, n);
    std::vector<std::tuple<int, int, int>> res;
    res.reserve(tab.size());
    for (auto [key, p] : tab)
        res.emplace_back(p, int(key >> 32), int(key & 0xffffffffULL));
    std::sort(res.begin(), res.end());
    return res;
}
