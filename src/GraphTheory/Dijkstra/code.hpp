#pragma once
#include "../../../Headers/Headers.hpp"

// SNIPPET BEGIN
template <class T, class G = std::common_type_t<T, long long>>
class Dijkstra {
    const std::vector<std::vector<std::pair<int, T>>> &adj;
    std::vector<std::vector<G>> dis;

    std::vector<G> get(int s) {
        std::vector<G> dis(adj.size(), Inf);

        using Pair = std::pair<G, int>;
        std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> q;

        dis[s] = G();
        q.push({dis[s], s});

        while (!q.empty()) {
            auto [d, x] = q.top();
            q.pop();

            if (d > dis[x]){
                continue;
            }

            for (auto [y, w] : adj[x]) {
                assert(w >= 0);
                if (w > Inf - d)
                    continue;
                if (dis[y] > dis[x] + w) {
                    dis[y] = dis[x] + w;
                    q.push({dis[y], y});
                }
            }
        }
        return dis;
    }

public:
    static constexpr G Inf = std::numeric_limits<G>::max();

    Dijkstra(const std::vector<std::vector<std::pair<int, T>>> &g)
        : adj(g), dis(g.size()) {}

    G operator()(int x, int y) {
        if (dis[x].empty())
            dis[x] = get(x);
        return dis[x][y];
    }
};

