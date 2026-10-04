#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "../StronglyConnectedComponent/Original.hpp"
#include "Original.hpp"
}
#include "Final.hpp"
template<class T> std::uint64_t checksum(T& problem) {
    if (!problem.work()) return 0;
    std::uint64_t sum = 1;
    for (std::size_t i = 0; i < problem.ans.size(); ++i) if (problem.ans[i]) sum += i + 1;
    return sum;
}
int main() {
    const int n = 20000;
    std::mt19937 rng(137);
    for (bool forced : {true, false}) {
        Legacy::TwoSat old(n);
        TwoSat current(n);
        if (forced) { old.assign(0, true); current.assign(0, true); }
        for (int x = 0; x + 1 < n; ++x) {
            old.add(x, true, x + 1, true);
            current.add(x, true, x + 1, true);
            if (!forced) for (int edge = 0; edge < 3; ++edge) {
                int y = rng() % n;
                old.add(x, true, y, true);
                current.add(x, true, y, true);
            }
        }
        compare(forced ? "forced-chain-20K" : "random-consistent-20K", [&] { return checksum(old); },
                [&] { return checksum(current); });
    }
}
