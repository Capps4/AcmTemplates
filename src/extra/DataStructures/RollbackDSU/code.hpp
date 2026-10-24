#pragma once
#include "../../../../Headers/Headers.hpp"

class RollbackDSU {
    struct Change {
        int x, y, siz;
    };
    std::vector<int> f;
    std::vector<Change> hist{};

public:
    explicit RollbackDSU(int n) : f(n, -1) {
        assert(n >= 0);
    }

    int find(int x) const {
        assert(0 <= x and x < int(f.size()));
        while (f[x] >= 0) {
            x = f[x];
        }
        return x;
    }

    int size(int x) const {
        return -f[find(x)];
    }

    bool same(int x, int y) const {
        return find(x) == find(y);
    }

    bool Union(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return false;
        }
        if (f[x] > f[y]) {
            std::swap(x, y);
        }
        hist.push_back({x, y, f[y]});
        f[x] += f[y];
        f[y] = x;
        return true;
    }

    std::size_t snapshot() const {
        return hist.size();
    }

    void rollback(std::size_t s) {
        assert(s <= hist.size());
        while (hist.size() > s) {
            auto [x, y, siz] = hist.back();
            hist.pop_back();
            f[x] -= siz;
            f[y] = siz;
        }
    }
};
