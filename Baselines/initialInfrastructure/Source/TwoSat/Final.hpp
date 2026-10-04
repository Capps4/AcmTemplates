#pragma once
#include <cassert>
#include <vector>
#include "../StronglyConnectedComponent/Final.hpp"

// SNIPPET BEGIN
class TwoSat {
    const int n;
    std::vector<std::vector<int>> adj;

public:
    std::vector<bool> ans;

    explicit TwoSat(int n) : n(n), adj(2 * n), ans(n) {}

    // (x == f) implies (y == g), including its contrapositive.
    void add(int x, bool f, int y, bool g) {
        assert(0 <= x && x < n && 0 <= y && y < n);
        adj[2 * x + f].push_back(2 * y + g);
        adj[2 * y + !g].push_back(2 * x + !f);
    }

    void assign(int x, bool v) {
        assert(0 <= x && x < n);
        adj[2 * x + !v].push_back(2 * x + v);
    }

    bool work() {
        Scc scc(adj, false);
        for (int i = 0; i < n; ++i)
            if (scc.bel[2 * i] == scc.bel[2 * i + 1]) {
                ans.clear();
                return false;
            }
        ans.resize(n);
        for (int i = 0; i < n; ++i)
            ans[i] = scc.bel[2 * i] > scc.bel[2 * i + 1];
        return true;
    }
};
