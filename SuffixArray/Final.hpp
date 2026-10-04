#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
class SuffixArray {
public:
    const int n;
    std::vector<int> sa, rk, h;

    explicit SuffixArray(const char *text) : SuffixArray(std::string_view(text)) {}

    template <class Sequence, class Compare = std::less<>,
              class = decltype(std::declval<const Sequence &>().size())>
    explicit SuffixArray(const Sequence &s, Compare cmp = {}) : n(s.size()), sa(n), rk(n), h(n) {
        if (n == 0) return;
        std::vector<int> id(n), tmp(n), cnt;
        using Value = std::decay_t<decltype(s[0])>;
        constexpr bool byteAlphabet =
            (std::is_same_v<Value, char> || std::is_same_v<Value, signed char> ||
             std::is_same_v<Value, unsigned char>) &&
            std::is_same_v<Compare, std::less<>>;
        auto eq = [&](int x, int y) {
            if constexpr (byteAlphabet)
                return s[x] == s[y];
            else
                return !cmp(s[x], s[y]) && !cmp(s[y], s[x]);
        };
        if constexpr (byteAlphabet) {
            cnt.assign(256, 0);
            for (int i = 0; i < n; ++i)
                ++cnt[static_cast<unsigned char>(s[i])];
            for (int i = 1; i < 256; ++i)
                cnt[i] += cnt[i - 1];
            for (int i = n - 1; i >= 0; --i)
                sa[--cnt[static_cast<unsigned char>(s[i])]] = i;
        } else {
            std::iota(sa.begin(), sa.end(), 0);
            std::sort(sa.begin(), sa.end(), [&](int x, int y) { return cmp(s[x], s[y]); });
        }
        int m = 1;
        rk[sa[0]] = 0;
        for (int i = 1; i < n; ++i) {
            m += !eq(sa[i - 1], sa[i]);
            rk[sa[i]] = m - 1;
        }
        for (int w = 1; m < n; w = w >= n - w ? n : 2 * w) {
            int p = 0;
            for (int i = n - w; i < n; ++i)
                id[p++] = i;
            for (int suffix : sa)
                if (suffix >= w) id[p++] = suffix - w;
            cnt.assign(m, 0);
            for (int suffix : id)
                ++cnt[rk[suffix]];
            for (int i = 1; i < m; ++i)
                cnt[i] += cnt[i - 1];
            for (int i = n - 1; i >= 0; --i)
                sa[--cnt[rk[id[i]]]] = id[i];
            int cnt = 1;
            tmp[sa[0]] = 0;
            for (int i = 1; i < n; ++i) {
                int x = sa[i - 1], y = sa[i];
                int rx = w < n - x ? rk[x + w] : -1;
                int ry = w < n - y ? rk[y + w] : -1;
                cnt += rk[x] != rk[y] || rx != ry;
                tmp[y] = cnt - 1;
            }
            rk.swap(tmp);
            m = cnt;
        }
        for (int i = 0, len = 0; i < n; ++i) {
            if (rk[i] == 0) {
                len = 0;
                continue;
            }
            if (len) --len;
            int pre = sa[rk[i] - 1];
            while (len < n - i && len < n - pre && eq(i + len, pre + len))
                ++len;
            h[rk[i]] = len;
        }
    }
};
