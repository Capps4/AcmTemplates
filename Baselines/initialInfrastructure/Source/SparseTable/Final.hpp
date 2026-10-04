#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

// SNIPPET BEGIN
template <class T, class Cmp = std::less<T>>
class RMQ {
    const int n;
    Cmp cmp;
    std::vector<std::vector<T>> jump{};

    static int floorLog(unsigned x) { return 31 - __builtin_clz(x); }

public:
    explicit RMQ(const std::vector<T> &a, Cmp cmp = {}) : n(a.size()), cmp(std::move(cmp)) {
        if (!n) return;
        int logn = floorLog(n);
        jump.resize(logn + 1);
        jump[0] = a;
        for (int j = 1; j <= logn; ++j) {
            int half = 1 << (j - 1), count = n - (1 << j) + 1;
            jump[j].resize(count);
            for (int i = 0; i < count; ++i)
                jump[j][i] = std::min(jump[j - 1][i], jump[j - 1][i + half], std::cref(this->cmp));
        }
    }

    // [l,r), returned by value as in Original.
    T operator()(int l, int r) const {
        assert(0 <= l && l < r && r <= n);
        int log = floorLog(r - l);
        return std::min(jump[log][l], jump[log][r - (1 << log)], std::cref(cmp));
    }
};
