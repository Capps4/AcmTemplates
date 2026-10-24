#pragma once
#include "../../../../Headers/Headers.hpp"

inline unsigned __int128 floorSum(std::uint64_t n, std::uint64_t m,
                                  std::uint64_t a, std::uint64_t b) {
    assert(m > 0);
    using U = unsigned __int128;
    U res = 0;
    auto add = [&](U x, std::uint64_t y) {
        if (y != 0 and x > (U(-1) - res) / y)
            throw std::overflow_error("FloorSum exceeds 128 bits");
        res += x * y;
    };
    while (n != 0) {
        add(U(n) * (n - 1) / 2, a / m);
        add(n, b / m);
        a %= m;
        b %= m;
        U y = U(a) * n + b;
        if (y < m)
            break;
        n = std::uint64_t(y / m);
        b = std::uint64_t(y % m);
        std::swap(a, m);
    }
    return res;
}
