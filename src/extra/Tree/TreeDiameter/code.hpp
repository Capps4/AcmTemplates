#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T>
class TreeDiameter {
    const std::vector<std::vector<T>>& e;
    std::vector<long long> dis;

    static std::pair<int, long long> edge(int x) { return {x, 1}; }

    template <class W>
    static std::pair<int, long long> edge(const std::pair<int, W>& p) {
        return {p.first, static_cast<long long>(p.second)};
    }

    int dfs(int r) {
        std::fill(dis.begin(), dis.end(), 0);
        std::vector<std::pair<int, int>> st{{r, -1}};
        int far = r;
        while (!st.empty()) {
            auto [x, p] = st.back();
            st.pop_back();
            if (dis[x] > dis[far]) {
                far = x;
            }
            for (const auto& v : e[x]) {
                auto [y, w] = edge(v);
                assert(w >= 0);
                if (y != p) {
                    assert(dis[x] <= std::numeric_limits<long long>::max() - w);
                    dis[y] = dis[x] + w;
                    st.emplace_back(y, x);
                }
            }
        }
        return far;
    }

public:
    int v1 = -1, v2 = -1;
    long long diameter = 0;

    explicit TreeDiameter(const std::vector<std::vector<T>>& adj) : e(adj), dis(adj.size(), 0) {
        if (!adj.empty()) {
            v1 = dfs(0);
            v2 = dfs(v1);
            diameter = dis[v2];
        }
    }
};
