#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T>
struct WeightedDSU {
    std::vector<int> f, size;
    std::vector<T> w;

    explicit WeightedDSU(int n) : f(n), size(n, 1), w(n, T{}) {
        std::iota(f.begin(), f.end(), 0);
    }

    int find(int x) {
        int r = x;
        T sum{};
        while (f[r] != r) {
            sum = sum + w[r];
            r = f[r];
        }
        while (f[x] != x) {
            int p = f[x];
            T v = w[x];
            f[x] = r;
            w[x] = sum;
            sum = sum - v;
            x = p;
        }
        return r;
    }

    // Enforce potential[y] - potential[x] = z; false means contradiction.
    bool Union(int x, int y, const T& z) {
        int a = find(x), b = find(y);
        T d = w[x] + z - w[y];
        if (a == b) {
            return d == T{};
        }
        if (size[a] < size[b]) {
            std::swap(a, b);
            d = T{} - d;
        }
        f[b] = a;
        w[b] = d;
        size[a] += size[b];
        return true;
    }
};

// replace me: additive potential.
struct Info {
    long long val;

    explicit Info(long long val = 0) : val(val) {}

    Info operator+(const Info& v) const {
        return Info(val + v.val);
    }

    Info operator-(const Info& v) const {
        return Info(val - v.val);
    }

    bool operator==(const Info& v) const {
        return val == v.val;
    }
};
