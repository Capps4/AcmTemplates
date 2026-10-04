#pragma once
#include <cassert>
#include <functional>
#include <limits>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
// Nonnegative weights, vertices 0..n-1. Cached g must remain unchanged.
template <class T, class G = std::common_type_t<T, long long>>
class Dijkstra {
    const std::vector<std::vector<std::pair<int, T>>> &adj;
    std::vector<std::vector<G>> cache;

    std::vector<G> get(int s) const {
        std::vector<G> dis(adj.size(), Inf);
        using Entry = std::pair<G, int>;
        std::priority_queue<Entry, std::vector<Entry>, std::greater<>> queue;
        dis[s] = 0;
        queue.emplace(0, s);
        while (!queue.empty()) {
            auto [nearDist, x] = queue.top();
            queue.pop();
            if (nearDist != dis[x]) continue;
            for (const auto &[y, c] : adj[x]) {
                G w = c;
                if (w <= Inf - nearDist && dis[y] > nearDist + w) {
                    dis[y] = nearDist + w;
                    queue.emplace(dis[y], y);
                }
            }
        }
        return dis;
    }

public:
    static constexpr G Inf = std::numeric_limits<G>::max();

    explicit Dijkstra(const std::vector<std::vector<std::pair<int, T>>> &g)
        : adj(g), cache(g.size()) {}

    const std::vector<G> &distances(int s) {
        assert(0 <= s && s < int(adj.size()));
        if (cache[s].empty()) cache[s] = get(s);
        return cache[s];
    }

    G operator()(int s, int t) {
        assert(0 <= t && t < int(adj.size()));
        return distances(s)[t];
    }
};
