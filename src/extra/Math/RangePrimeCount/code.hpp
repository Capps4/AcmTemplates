#pragma once
#include "../../../../Headers/Headers.hpp"

inline long long cntPrimeLessEqual(long long n) {
    if (n <= 1) {
        return 0;
    }
    long long m = static_cast<long long>(std::sqrt(static_cast<long double>(n)));
    while (m + 1 <= n / (m + 1)) {
        ++m;
    }
    while (m > n / m) {
        --m;
    }
    // Vector indices and the final f loop use int.
    assert(m < std::numeric_limits<int>::max());
    int s = int(m);
    std::vector<int> f(s + 1);
    std::vector<long long> g(s + 1);
    for (int i = 1; i <= s; ++i) {
        f[i] = i;
        g[i] = n / i;
    }
    for (int i = 2; i <= s; ++i) {
        if (f[i] == f[i - 1]) {
            continue;
        }
        int x = f[i - 1];
        long long sq = 1LL * i * i;
        int u = int(std::min(m, n / sq)), d = std::min(u, s / i);
        for (int j = 1; j <= d; ++j) {
            g[j] -= g[i * j] - x;
        }
        long long t = n / i;
        for (int j = d + 1; j <= u; ++j) {
            g[j] -= f[std::size_t(t / j)] - x;
        }
        for (int j = s; j >= sq; --j) {
            f[j] -= f[j / i] - x;
        }
    }
    return g[1] - 1;
}
