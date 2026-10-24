#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T, class Key = int>
class SparseMatBIT {
    std::vector<Key> xs{};
    std::vector<std::vector<Key>> ys{};
    std::vector<std::vector<T>> t{};

public:
    explicit SparseMatBIT(const std::vector<std::pair<Key, Key>>& pts) {
        xs.reserve(pts.size());
        for (const auto& p : pts) {
            xs.push_back(p.first);
        }
        std::sort(xs.begin(), xs.end());
        xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
        ys.resize(xs.size() + 1);
        t.resize(xs.size() + 1);
        for (const auto& [x, y] : pts) {
            auto at = std::lower_bound(xs.begin(), xs.end(), x) - xs.begin();
            for (std::size_t i = at + 1; i < ys.size(); i += i & -i) {
                ys[i].push_back(y);
            }
        }
        for (std::size_t i = 1; i < ys.size(); ++i) {
            auto& a = ys[i];
            std::sort(a.begin(), a.end());
            a.erase(std::unique(a.begin(), a.end()), a.end());
            t[i].resize(a.size() + 1);
        }
    }

    void modify(Key x, Key y, const T& v) {
        auto at = std::lower_bound(xs.begin(), xs.end(), x);
        assert(at != xs.end() and *at == x);
        for (std::size_t i = at - xs.begin() + 1; i < ys.size(); i += i & -i) {
            auto it = std::lower_bound(ys[i].begin(), ys[i].end(), y);
            assert(it != ys[i].end() and *it == y);
            for (std::size_t j = it - ys[i].begin() + 1; j < t[i].size(); j += j & -j) {
                t[i][j] = t[i][j] + v;
            }
        }
    }

    T prefixQuery(Key x, Key y) const {
        T sum{};
        auto at = std::lower_bound(xs.begin(), xs.end(), x) - xs.begin();
        for (std::size_t i = at; i > 0; i -= i & -i) {
            auto it = std::lower_bound(ys[i].begin(), ys[i].end(), y) - ys[i].begin();
            for (std::size_t j = it; j > 0; j -= j & -j) {
                sum = sum + t[i][j];
            }
        }
        return sum;
    }

    T rangeQuery(Key xl, Key yl, Key xr, Key yr) const {
        assert(not (xr < xl) and not (yr < yl));
        return prefixQuery(xr, yr) - prefixQuery(xl, yr)
             - prefixQuery(xr, yl) + prefixQuery(xl, yl);
    }
};
