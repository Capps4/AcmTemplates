#pragma once
#include <bits/stdc++.h>
using i64 = long long;
class SuffixArray {
public:
    const int n;
    std::vector<int> sa, rk, h{};

    template <class T>
    SuffixArray(const T &s)
        : n(s.size()), sa(n), rk(n), id(n), tmp(n) {

        std::iota(begin(id), end(id), 0);
        for (int i = 0; i < n; i++)
            rk[i] = s[i];

        countSort();

        for (int w = 1;; w <<= 1) {
            std::iota(begin(id), begin(id) + w + 1, n - w);
            for (int i = 0, p = w; i < n; i++)
                if (sa[i] >= w)
                    id[p++] = sa[i] - w;

            countSort();
            oldrk = rk;

            rk[sa[0]] = 0;
            for (int i = 1, p = 0; i < n; i++)
                rk[sa[i]] = equal(sa[i], sa[i - 1], w) ? p : ++p;

            if (rk[sa.back()] + 1 == n)
                break;
        }

        calcHeight(s);
    }

private:
    std::vector<int> oldrk{}, id, tmp, cnt{};

    template <class T>
    void calcHeight(const T &s) {
        h.assign(n, 0);
        for (int i = 0, k = 0; i < n; i++) {
            if (rk[i] == 0)
                continue;
            k -= bool(k);
            while (s[i + k] == s[sa[rk[i] - 1] + k])
                k += 1;
            h[rk[i]] = k;
        }
    }

    // 计数排序
    void countSort() {
        int m = *std::max_element(begin(rk), end(rk));
        cnt.assign(m + 1, 0);
        for (int i = 0; i < n; i++)
            cnt[tmp[i] = rk[id[i]]] += 1;
        for (auto i = 1U; i < cnt.size(); i++)
            cnt[i] += cnt[i - 1];
        for (int i = n - 1; i >= 0; i--)
            sa[--cnt[tmp[i]]] = id[i];
    }

    bool equal(int x, int y, int w) {
        int rkx = (x + w < n ? oldrk[x + w] : -1);
        int rky = (y + w < n ? oldrk[y + w] : -1);
        return oldrk[x] == oldrk[y] and rkx == rky;
    }

    /**
     * sa[i] -> 第i小的后缀
     * rk[i] -> 后缀i的排名
     */
};

