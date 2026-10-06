#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
class SuffixArray {
public:
    const int n;
    std::vector<int> sa, rk, h;

    explicit SuffixArray(const char *s) : SuffixArray(std::string_view(s)) {}

    template <class T>
    explicit SuffixArray(const T &s) : n(s.size()), sa(n), rk(n), h(n), id(n), tmp(n) {
        if (n == 0)
            return;
        std::iota(sa.begin(), sa.end(), 0);
        using Value = std::decay_t<decltype(s[0])>;
        constexpr bool byte = sizeof(Value) == 1 and std::is_integral_v<Value>;
        if constexpr (byte) {
            std::iota(id.begin(), id.end(), 0);
            for (int i = 0; i < n; ++i)
                rk[i] = static_cast<unsigned char>(s[i]);
            countSort();
        } else {
            std::sort(sa.begin(), sa.end(), [&](int x, int y) { return s[x] < s[y]; });
        }
        int m = 1;
        rk[sa[0]] = 0;
        for (int i = 1; i < n; ++i) {
            m += !(s[sa[i]] == s[sa[i - 1]]);
            rk[sa[i]] = m - 1;
        }
        for (int w = 1; m < n; w = w >= n - w ? n : 2 * w) {
            std::iota(id.begin(), id.begin() + w, n - w);
            for (int i = 0, p = w; i < n; ++i)
                if (sa[i] >= w)
                    id[p++] = sa[i] - w;
            countSort();
            oldrk = rk;
            rk[sa[0]] = 0;
            m = 1;
            for (int i = 1; i < n; ++i) {
                m += !equal(sa[i], sa[i - 1], w);
                rk[sa[i]] = m - 1;
            }
        }
        calcHeight(s);
    }

private:
    std::vector<int> oldrk{}, id, tmp, cnt{};

    template <class T>
    void calcHeight(const T &s) {
        for (int i = 0, k = 0; i < n; ++i) {
            if (rk[i] == 0) {
                k = 0;
                continue;
            }
            k -= bool(k);
            int j = sa[rk[i] - 1];
            while (k < n - i and k < n - j and s[i + k] == s[j + k])
                ++k;
            h[rk[i]] = k;
        }
    }

    void countSort() {
        int m = *std::max_element(rk.begin(), rk.end()) + 1;
        cnt.assign(m, 0);
        for (int i = 0; i < n; ++i)
            ++cnt[tmp[i] = rk[id[i]]];
        for (int i = 1; i < m; ++i)
            cnt[i] += cnt[i - 1];
        for (int i = n - 1; i >= 0; --i)
            sa[--cnt[tmp[i]]] = id[i];
    }

    bool equal(int x, int y, int w) const {
        int a = w < n - x ? oldrk[x + w] : -1;
        int b = w < n - y ? oldrk[y + w] : -1;
        return oldrk[x] == oldrk[y] and a == b;
    }
};
