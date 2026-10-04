#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

// SNIPPET BEGIN
struct IdentityKey {
    template <class T>
    constexpr int operator()(const T &x) const {
        return x;
    }
};

// Stable index res; keys are in [0, mx]. O(n + mx).
template <class T, class Key = IdentityKey>
std::vector<int> countingSortOrder(const std::vector<T> &a, Key key = {}) {
    if (a.empty()) return {};
    int mx = 0;
    for (const auto &x : a) {
        int k = key(x);
        assert(k >= 0);
        mx = std::max(mx, k);
    }
    std::vector<int> cnt(mx + 1), res(a.size());
    for (const auto &x : a)
        ++cnt[key(x)];
    for (int i = 1; i < int(cnt.size()); ++i)
        cnt[i] += cnt[i - 1];
    for (int i = int(a.size()); i-- > 0;)
        res[--cnt[key(a[i])]] = i;
    return res;
}
