#pragma once
#include <bits/stdc++.h>
using i64 = long long;
inline auto minimalString() {
    return seq::Op{[](auto&& input) {
        auto a = std::forward<decltype(input)>(input);
        const std::size_t n = a.size();
        std::size_t i = 0, j = 1, k = 0;
        while (k < n && i < n && j < n) {
            if (a[(i + k) % n] == a[(j + k) % n]) ++k;
            else {
                (a[(i + k) % n] > a[(j + k) % n] ? i : j) += k + 1;
                i += i == j;
                k = 0;
            }
        }
        std::rotate(a.begin(), a.begin() + std::min(i, j), a.end());
        return a;
    }};
}

