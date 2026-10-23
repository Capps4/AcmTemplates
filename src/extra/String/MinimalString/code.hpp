#pragma once
#include "Include.hpp"

// SNIPPET BEGIN
inline auto minimalString() {
    return seq::Op{[](auto &&src) {
        auto a = [&] {
            if constexpr (std::is_same_v<std::decay_t<decltype(src)>, std::string_view>)
                return std::string(src);
            else
                return std::forward<decltype(src)>(src);
        }();
        const std::size_t n = a.size();
        if (!n)
            return a;
        auto at = [&](std::size_t pos) {
            using T = typename decltype(a)::value_type;
            if constexpr (std::is_same_v<T, char>)
                return static_cast<unsigned char>(a[pos]);
            else
                return a[pos];
        };
        std::size_t i = 0, j = 1, k = 0;
        while (k < n and i < n and j < n) {
            auto l = at(i + k < n ? i + k : i + k - n);
            auto r = at(j + k < n ? j + k : j + k - n);
            if (l == r)
                ++k;
            else {
                (l > r ? i : j) += k + 1;
                i += i == j;
                k = 0;
            }
        }
        std::rotate(a.begin(), a.begin() + std::min(i, j), a.end());
        return a;
    }};
}
