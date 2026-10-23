#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T, class Cmp = std::less<T>>
class BitRMQ {
    static constexpr int B = 64;
    using U = std::uint64_t;
    const int n;
    Cmp cmp;
    std::vector<T> ini, pre, suf;
    std::vector<U> stk;
    std::vector<std::vector<T>> a{};

    T best(const T& x, const T& y) const { return cmp(y, x) ? y : x; }

public:
    explicit BitRMQ(const std::vector<T>& v, Cmp cmp = {})
        : n(int(v.size())), cmp(std::move(cmp)), ini(v), pre(v), suf(v), stk(n) {
        if (n == 0) {
            return;
        }
        int m = (n - 1) / B + 1, lg = 31 - __builtin_clz(unsigned(m));
        std::vector<T> row{};
        row.reserve(m);
        for (int l = 0; l < n; l += B) {
            T val = v[l];
            U s = 0;
            for (int j = l; j < std::min(n, l + B); ++j) {
                val = best(val, v[j]);
                while (s != 0 and this->cmp(v[j], v[l + 63 - __builtin_clzll(s)])) {
                    s &= ~(U(1) << (63 - __builtin_clzll(s)));
                }
                s |= U(1) << (j - l);
                stk[j] = s;
            }
            row.push_back(val);
        }
        a.push_back(std::move(row));
        for (int j = 1; j <= lg; ++j) {
            std::vector<T> next{};
            for (int i = 0; i + (1 << j) <= m; ++i) {
                next.push_back(best(a[j - 1][i], a[j - 1][i + (1 << (j - 1))]));
            }
            a.push_back(std::move(next));
        }
        for (int i = 1; i < n; ++i) {
            if (i % B != 0) {
                pre[i] = best(pre[i - 1], pre[i]);
            }
        }
        for (int i = n - 2; i >= 0; --i) {
            if (i % B != B - 1) {
                suf[i] = best(suf[i], suf[i + 1]);
            }
        }
    }

    T operator()(int l, int r) const {
        assert(0 <= l and l < r and r <= n);
        if (l / B == (r - 1) / B) {
            U s = stk[r - 1] >> (l % B);
            return ini[l + __builtin_ctzll(s)];
        }
        T ans = best(suf[l], pre[r - 1]);
        l = l / B + 1;
        r /= B;
        if (l < r) {
            int k = 31 - __builtin_clz(unsigned(r - l));
            ans = best(ans, best(a[k][l], a[k][r - (1 << k)]));
        }
        return ans;
    }
};
