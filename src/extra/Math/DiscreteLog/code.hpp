#pragma once
#include "../../../../Headers/Headers.hpp"

inline long long discreteLog(long long a, long long b, int m) {
    using i64 = long long;
    assert(m >= 1);
    a %= m;
    b %= m;
    if (a < 0)
        a += m;
    if (b < 0)
        b += m;
    i64 cur = 1 % m;
    int add = 0;
    while (true) {
        if (b == cur)
            return add;
        i64 g = std::gcd(a, i64(m));
        if (g == 1)
            break;
        if (b % g != 0)
            return -1;
        b /= g;
        m /= int(g);
        cur = cur * (a / g) % m;
        ++add;
    }
    int n = int(std::sqrt(m)) + 1;
    i64 mul = 1, val = b;
    std::unordered_map<int, int> tab;
    tab.reserve(n + 1);
    for (int j = 0; j <= n; ++j) {
        // Keeping the largest j minimizes i*n-j at the first matching block.
        tab[int(val)] = j;
        val = val * a % m;
        if (j < n)
            mul = mul * a % m;
    }
    for (int i = 1; i <= n; ++i) {
        cur = cur * mul % m;
        auto it = tab.find(int(cur));
        if (it != tab.end())
            return i64(i) * n - it->second + add;
    }
    return -1;
}
