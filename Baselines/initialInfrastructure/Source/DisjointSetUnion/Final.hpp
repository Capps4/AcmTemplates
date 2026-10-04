#pragma once
#include <numeric>
#include <utility>
#include <vector>

// SNIPPET BEGIN
class DSU {
    std::vector<int> f{};

public:
    std::vector<int> size{};

    explicit DSU(int n = 0) { init(n); }

    void init(int n) {
        f.resize(n);
        std::iota(f.begin(), f.end(), 0);
        size.assign(n, 1);
    }

    int find(int x) {
        while (x != f[x])
            x = f[x] = f[f[x]];
        return x;
    }

    void Union(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) return;
        // 如需按大小合并，取消下面三行的注释。
        // if (size[x] < size[y]) {
        //     std::swap(x, y);
        // }
        size[x] += size[y];
        f[y] = x;
    }
};
