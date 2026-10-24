#pragma once
#include "../../../../Headers/Headers.hpp"

template <class Line, class Cmp = std::greater<typename Line::Value>>
class LiChaoTree {
    using Key = typename Line::Key;
    std::vector<Key> keys;
    std::vector<Line> a{};
    std::vector<int> t;
    Cmp cmp;

    auto val(int id, int x) const { return a[id - 1][keys[x]]; }

    void put(int p, int l, int r, int id) {
        if (t[p] == 0) {
            t[p] = id;
            return;
        }
        int mid = l + (r - l) / 2;
        bool left = cmp(val(id, l), val(t[p], l));
        bool midv = cmp(val(id, mid), val(t[p], mid));
        if (midv) {
            std::swap(t[p], id);
        }
        if (l == r) {
            return;
        }
        if (left != midv) {
            put(p * 2, l, mid, id);
        } else {
            put(p * 2 + 1, mid + 1, r, id);
        }
    }

    void insert(int p, int l, int r, int u, int v, int id) {
        if (u <= l and r <= v) {
            put(p, l, r, id);
            return;
        }
        int mid = l + (r - l) / 2;
        if (u <= mid) {
            insert(p * 2, l, mid, u, v, id);
        }
        if (v > mid) {
            insert(p * 2 + 1, mid + 1, r, u, v, id);
        }
    }

    int query(int p, int l, int r, int x) const {
        int ans = t[p];
        if (l != r) {
            int mid = l + (r - l) / 2;
            int id = x <= mid ? query(p * 2, l, mid, x) : query(p * 2 + 1, mid + 1, r, x);
            if (id != 0 and (ans == 0 or cmp(val(id, x), val(ans, x)))) {
                ans = id;
            }
        }
        return ans;
    }

public:
    explicit LiChaoTree(const std::vector<Key>& xs, Cmp cmp = {})
        : keys(xs), t(4 * std::max(std::size_t(1), xs.size()), 0), cmp(std::move(cmp)) {
        for (std::size_t i = 1; i < keys.size(); ++i) {
            assert(keys[i - 1] < keys[i]);
        }
    }

    // Half-open indices [l, r) in the registered key set.
    void insert(int l, int r, Line line) {
        assert(0 <= l and l <= r and r <= int(keys.size()));
        if (l == r) {
            return;
        }
        a.push_back(std::move(line));
        insert(1, 0, int(keys.size()) - 1, l, r - 1, int(a.size()));
    }

    Line queryLine(Key key) const {
        auto it = std::lower_bound(keys.begin(), keys.end(), key);
        assert(it != keys.end() and *it == key);
        int id = query(1, 0, int(keys.size()) - 1, int(it - keys.begin()));
        assert(id != 0);
        return a[id - 1];
    }

    typename Line::Value query(Key key) const { return queryLine(key)[key]; }
};
