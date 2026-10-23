#pragma once
#include "../../../../Headers/Headers.hpp"

struct DirectedEulerRoad {
    const int n;
    std::vector<int> ans{}, cur;
    std::string roadType = "NoSolution";

    explicit DirectedEulerRoad(const std::vector<std::vector<int>>& adj)
        : n(int(adj.size())), cur(n, 0) {
        std::vector<int> deg(n, 0);
        std::size_t m = 0;
        for (int x = 0; x < n; ++x) {
            m += adj[x].size();
            deg[x] += int(adj[x].size());
            for (int y : adj[x]) {
                assert(y >= 0 and y < n);
                --deg[y];
            }
        }
        int src = -1, dst = -1, beg = 0;
        for (int x = 0; x < n; ++x) {
            if (!adj[x].empty()) {
                beg = x;
            }
            if (deg[x] == 1 and src == -1) {
                src = x;
            } else if (deg[x] == -1 and dst == -1) {
                dst = x;
            } else if (deg[x] != 0) {
                return;
            }
        }
        if ((src == -1) != (dst == -1)) {
            return;
        }
        if (src != -1) {
            beg = src;
        }
        if (n != 0) {
            std::vector<int> st{beg};
            while (!st.empty()) {
                int x = st.back();
                if (cur[x] < int(adj[x].size())) {
                    st.push_back(adj[x][cur[x]++]);
                } else {
                    ans.push_back(x);
                    st.pop_back();
                }
            }
        }
        if (ans.size() != m + (n != 0)) {
            ans.clear();
            return;
        }
        std::reverse(ans.begin(), ans.end());
        roadType = src == -1 ? "EulerCircuit" : "EulerPath";
    }
};
