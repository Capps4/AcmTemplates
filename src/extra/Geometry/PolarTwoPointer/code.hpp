#pragma once
#include "Include.hpp"

template <class T>
std::vector<int> twoPointerWithPolarSorting(std::vector<Vec<T>>& a) {
    int n = int(a.size());
    for (const auto& v : a) {
        assert(v != Vec<T>::O);
    }
    std::stable_sort(a.begin(), a.end(), [](const auto& x, const auto& y) {
        return x.half() != y.half() ? x.half() < y.half() : x.cross(y) > 0;
    });
    std::vector<int> ans(n);
    int r = 1;
    for (int l = 0; l < n; ++l) {
        r = std::max(r, l + 1);
        while (r < l + n) {
            auto c = a[l].cross(a[r % n]);
            if (c < 0 or (c == 0 and a[l].dot(a[r % n]) <= 0)) {
                break;
            }
            ++r;
        }
        ans[l] = r % n;
    }
    return ans;
}
