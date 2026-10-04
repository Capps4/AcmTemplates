#pragma once
#include <algorithm>
#include <type_traits>
#include "../ListHelper/Final.hpp"

// SNIPPET BEGIN
inline auto minimalString() {
    return seq::Op{[](auto &&input) {
        auto a = seq::materialize(std::forward<decltype(input)>(input));
        const std::size_t n = a.size();
        if (!n) return a;
        auto at = [&](std::size_t pos) {
            using T = typename decltype(a)::value_type;
            if constexpr (std::is_same_v<T, char>)
                return static_cast<unsigned char>(a[pos]);
            else
                return a[pos];
        };
        std::size_t i = 0, j = 1, k = 0;
        while (k < n && i < n && j < n) {
            auto left = at(i + k < n ? i + k : i + k - n);
            auto right = at(j + k < n ? j + k : j + k - n);
            if (left == right)
                ++k;
            else {
                (left > right ? i : j) += k + 1;
                i += i == j;
                k = 0;
            }
        }
        std::rotate(a.begin(), a.begin() + std::min(i, j), a.end());
        return a;
    }};
}
