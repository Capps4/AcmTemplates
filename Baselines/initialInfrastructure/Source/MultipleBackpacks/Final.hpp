#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>

// SNIPPET BEGIN
using VecGood = std::vector<std::array<int, 3>>; // {value, w, cnt}

inline std::vector<long long> multiBag(const VecGood &goods, int cap) {
    assert(cap >= 0);
    std::vector<long long> best(cap + 1);
    std::vector<int> queue(cap + 2);
    for (const auto &[value, w, available] : goods) {
        assert(w >= 0 && available >= 0);
        if (available == 0 || value <= 0) continue;
        if (w == 0) {
            long long bonus = 1LL * value * available;
            for (auto &result : best)
                result += bonus;
            continue;
        }
        if (w > cap) continue;
        int cnt = std::min(available, cap / w);
        auto score = [&](int target, int source) {
            return best[source] + 1LL * ((target - source) / w) * value;
        };
        // Descending residue chains preserve unmodified source DP values.
        for (int top = cap; top > cap - w; --top) {
            int first = 1, last = 0, source = top;
            for (int target = top; target > 0; target -= w) {
                long long lower = std::max(0LL, target - 1LL * cnt * w);
                for (; source >= lower; source -= w) {
                    while (first <= last && score(target, source) > score(target, queue[last]))
                        --last;
                    queue[++last] = source;
                }
                best[target] = score(target, queue[first]);
                if (queue[first] == target) ++first;
            }
        }
    }
    return best;
}
