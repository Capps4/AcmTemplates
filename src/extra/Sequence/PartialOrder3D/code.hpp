#pragma once
#include "Include.hpp"

template <int D = 3, class T = long long>
std::vector<T> partialOrder(std::vector<std::array<int, D>> a, const std::vector<int>& w) {
    static_assert(D == 3);
    assert(a.size() == w.size());
    int n = int(a.size());
    auto vals = a;
    std::sort(vals.begin(), vals.end());
    vals.erase(std::unique(vals.begin(), vals.end()), vals.end());
    int m = int(vals.size());
    std::vector<int> idx(n), ord(m), z(m), keys{};
    std::vector<T> sum(m, T{}), ans(m, T{});
    for (int i = 0; i < n; ++i) {
        idx[i] = int(std::lower_bound(vals.begin(), vals.end(), a[i]) - vals.begin());
        sum[idx[i]] += T(w[i]);
    }
    for (const auto& p : vals) {
        keys.push_back(p[2]);
    }
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    for (int i = 0; i < m; ++i) {
        z[i] = int(std::lower_bound(keys.begin(), keys.end(), vals[i][2]) - keys.begin());
    }
    std::iota(ord.begin(), ord.end(), 0);
    ans = sum;
    Fenwick<T> tr(int(keys.size()));
    auto cmp = [&](int x, int y) { return vals[x][1] < vals[y][1]; };
    auto work = [&](auto&& self, int l, int r) -> void {
        if (r - l <= 1) {
            return;
        }
        int mid = l + (r - l) / 2;
        self(self, l, mid);
        self(self, mid, r);
        int i = l;
        for (int j = mid; j < r; ++j) {
            while (i < mid and vals[ord[i]][1] <= vals[ord[j]][1]) {
                tr.modify(z[ord[i]], sum[ord[i]]);
                ++i;
            }
            ans[ord[j]] += tr.query(z[ord[j]]);
        }
        for (int k = l; k < i; ++k) {
            tr.modify(z[ord[k]], T{} - sum[ord[k]]);
        }
        std::inplace_merge(ord.begin() + l, ord.begin() + mid, ord.begin() + r, cmp);
    };
    work(work, 0, m);
    std::vector<T> res(n);
    for (int i = 0; i < n; ++i) {
        res[i] = ans[idx[i]];
    }
    return res;
}
