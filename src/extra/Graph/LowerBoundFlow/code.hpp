#pragma once
#include "Include.hpp"

// SNIPPET BEGIN
template <class T>
class LowerBoundFlow {
    static_assert(std::is_integral_v<T> and std::is_signed_v<T>);

    struct Edge {
        int s, t;
        T lo, hi;
    };
    int n;
    T sum = 0;
    std::vector<Edge> ed{};
    std::vector<T> ans{};
    bool ok = false;

    std::optional<T> run(int s, int t) {
        ok = false;
        ans.clear();
        Flow<T> f(n + 2);
        std::vector<T> bal(n);
        for (const auto &[x, y, lo, hi] : ed) {
            f.add(x, y, hi - lo);
            bal[x] -= lo;
            bal[y] += lo;
        }
        int cut = int(f.adj.size());
        int back = -1;
        if (s != -1)
            back = f.add(t, s, sum);
        T need = 0;
        for (int x = 0; x < n; ++x) {
            if (bal[x] > 0) {
                f.add(n, x, bal[x]);
                need += bal[x];
            } else if (bal[x] < 0)
                f.add(x, n + 1, -bal[x]);
        }
        if (f.work(n, n + 1, need) != need)
            return std::nullopt;
        T res = back == -1 ? 0 : f.adj[back ^ 1].second;
        for (int id = cut; id < int(f.adj.size()); ++id)
            f.adj[id].second = 0;
        if (s != -1)
            res += f.work(s, t, sum - res);
        ans.reserve(ed.size());
        for (int i = 0; i < int(ed.size()); ++i)
            ans.push_back(ed[i].lo + f.adj[2 * i + 1].second);
        ok = true;
        return res;
    }

public:
    explicit LowerBoundFlow(int n) : n(n) {
        assert(0 <= n and n <= std::numeric_limits<int>::max() - 2);
    }

    int add(int s, int t, T lo, T hi) {
        assert(0 <= s and s < n and 0 <= t and t < n);
        assert(0 <= lo and lo <= hi and hi <= std::numeric_limits<T>::max() - sum);
        assert(ed.size() + std::size_t(n) + 1 < std::size_t(std::numeric_limits<int>::max() / 2));
        int id = int(ed.size());
        ed.push_back({s, t, lo, hi});
        sum += hi;
        ok = false;
        return id;
    }

    bool work() {
        return run(-1, -1).has_value();
    }

    std::optional<T> work(int s, int t) {
        assert(0 <= s and s < n and 0 <= t and t < n and s != t);
        return run(s, t);
    }

    T get(int id) const {
        assert(ok and 0 <= id and id < int(ed.size()));
        return ans[id];
    }
};
