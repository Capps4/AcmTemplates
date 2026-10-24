#pragma once
#include "../../../../Support/BenchmarkSupport.hpp"
#include <numeric>

inline std::vector<std::pair<int, int>> edges(const BenchmarkInput &in) {
    std::vector<std::pair<int, int>> ed;
    if (in.shape == 0) {
        for (int x = 0; x + 1 < in.n; ++x) {
            ed.emplace_back(x, x);
            ed.emplace_back(x, x + 1);
        }
        ed.emplace_back(in.n - 1, 0);
    } else {
        std::mt19937_64 rng(in.seed);
        std::vector<int> ids(in.n);
        std::iota(ids.begin(), ids.end(), 0);
        std::shuffle(ids.begin(), ids.end(), rng);
        int len = 2;
        for (int l = 0; l < in.n; ) {
            int r = std::min(in.n, l + len);
            // Greedy matching leaves the last left vertex unmatched. Each
            // block needs a different augmenting-path depth after phase one.
            for (int x = l; x + 1 < r; ++x) {
                ed.emplace_back(x, ids[x]);
                ed.emplace_back(x, ids[x + 1]);
                ed.emplace_back(x, ids[l + int(rng() % (x - l + 1))]);
            }
            ed.emplace_back(r - 1, ids[l]);
            l = r;
            len = len == 64 ? 2 : len * 2;
        }
    }
    return ed;
}
