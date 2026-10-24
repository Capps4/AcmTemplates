#pragma once
#include "../../../../Support/BenchmarkSupport.hpp"

inline std::vector<std::vector<int>> graph(const BenchmarkInput &in) {
    std::vector<std::vector<int>> g(in.n);
    for (int x = 1; x < in.n; ++x) {
        int p = in.shape ? 0 : x - 1;
        g[x].push_back(p);
        g[p].push_back(x);
    }
    return g;
}

inline long long expected(const BenchmarkInput &in, long long x) {
    if (in.shape)
        return x == 0 ? in.n - 1 : 2LL * in.n - 3;
    return x * (x + 1) / 2 + (in.n - x - 1) * (in.n - x) / 2;
}
