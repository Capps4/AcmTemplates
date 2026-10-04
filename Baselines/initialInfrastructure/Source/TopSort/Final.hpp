#pragma once
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

// SNIPPET BEGIN
class TopSort {
    static constexpr int endPoint(int x) { return x; }

    template <class Weight>
    static constexpr int endPoint(const std::pair<int, Weight> &edge) {
        return edge.first;
    }

public:
    template <class Row>
    std::vector<int> operator()(const std::vector<Row> &adj) const {
        int n = int(adj.size());
        std::vector<int> deg(n);
        for (const auto &row : adj)
            for (const auto &edge : row) {
                int x = endPoint(edge);
                assert(0 <= x && x < n);
                ++deg[x];
            }
        std::vector<int> res;
        res.reserve(n);
        for (int x = 0; x < n; ++x)
            if (!deg[x]) res.push_back(x);
        for (int i = 0; i < int(res.size()); ++i) {
            int x = res[i];
            for (const auto &edge : adj[x]) {
                int y = endPoint(edge);
                if (--deg[y] == 0) res.push_back(y);
            }
        }
        return res; // res.size() == adj.size() iff the graph is acyclic.
    }
};

inline constexpr TopSort topSort{};
