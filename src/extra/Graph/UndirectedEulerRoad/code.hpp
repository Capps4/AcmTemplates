#pragma once
#include "../../../../Headers/Headers.hpp"

struct XorGraph {
    const int n;
    std::vector<int> edg{};
    std::vector<std::vector<int>> adj;

    explicit XorGraph(int n) : n(n), adj(n) { assert(n >= 0); }

    void add(int x, int y) {
        assert(x >= 0 and x < n and y >= 0 and y < n);
        int id = int(edg.size());
        edg.push_back(x ^ y);
        adj[x].push_back(id);
        adj[y].push_back(id);
    }
};

struct UndirectedEulerRoad : XorGraph {
    std::vector<int> ans{};
    std::string roadType = "NoSolution";

    explicit UndirectedEulerRoad(int n) : XorGraph(n) {}
    explicit UndirectedEulerRoad(const XorGraph& g) : XorGraph(g) {}

    std::string work() {
        ans.clear();
        roadType = "NoSolution";
        std::vector<int> odd{}, cur(n, 0);
        int beg = 0;
        for (int x = 0; x < n; ++x) {
            if (!adj[x].empty()) {
                beg = x;
            }
            if (adj[x].size() % 2 != 0) {
                odd.push_back(x);
            }
        }
        if (!odd.empty() and odd.size() != 2) {
            return roadType;
        }
        if (!odd.empty()) {
            beg = odd.front();
        }
        std::vector<bool> vis(edg.size(), false);
        if (n != 0) {
            std::vector<int> st{beg};
            while (!st.empty()) {
                int x = st.back();
                while (cur[x] < int(adj[x].size()) and vis[adj[x][cur[x]]]) {
                    ++cur[x];
                }
                if (cur[x] == int(adj[x].size())) {
                    ans.push_back(x);
                    st.pop_back();
                } else {
                    int id = adj[x][cur[x]++];
                    vis[id] = true;
                    st.push_back(x ^ edg[id]);
                }
            }
        }
        if (ans.size() != edg.size() + (n != 0)) {
            ans.clear();
            return roadType;
        }
        std::reverse(ans.begin(), ans.end());
        roadType = odd.empty() ? "EulerCircuit" : "EulerPath";
        return roadType;
    }
};
