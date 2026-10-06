#pragma once
#include "../../../Headers/Headers.hpp"

// SNIPPET BEGIN
using VecGood = std::vector<std::array<int, 3>>; // {value, w, cnt}

inline std::vector<long long> multiBag(const VecGood &gs, int cap) {
    assert(cap >= 0);
    std::vector<long long> best(cap + 1);
    std::vector<int> q(cap + 2);
    for (const auto &[val, w, num] : gs) {
        assert(w >= 0 and num >= 0);
        if (num == 0 or val <= 0)
            continue;
        if (w == 0) {
            long long add = 1LL * val * num;
            for (auto &res : best)
                res += add;
            continue;
        }
        if (w > cap)
            continue;
        int cnt = std::min(num, cap / w);
        auto calc = [&](int dst, int src) {
            return best[src] + 1LL * ((dst - src) / w) * val;
        };
        // Descending residue chains preserve unmodified source DP values.
        for (int top = cap; top > cap - w; --top) {
            int l = 1, last = 0, src = top;
            for (int dst = top; dst > 0; dst -= w) {
                long long low = std::max(0LL, dst - 1LL * cnt * w);
                for (; src >= low; src -= w) {
                    while (l <= last and calc(dst, src) > calc(dst, q[last]))
                        --last;
                    q[++last] = src;
                }
                best[dst] = calc(dst, q[l]);
                if (q[l] == dst)
                    ++l;
            }
        }
    }
    return best;
}
