#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
class CentroidDecomposition {
public:
    std::vector<int> dfsOrder{};

    explicit CentroidDecomposition(const std::vector<std::vector<int>> &adj,
                                   std::vector<std::vector<int>> *cdt = nullptr) {
        int n = int(adj.size());
        assert(cdt != &adj);
        if (cdt != nullptr)
            cdt->assign(n, {});
        std::vector<char> ban(n);
        std::vector<int> fa(n), siz(n), vs;
        std::vector<std::pair<int, int>> q;
        vs.reserve(n);
        q.reserve(n);
        dfsOrder.reserve(n);
        for (int root = 0; root < n; ++root) {
            if (ban[root])
                continue;
            q.emplace_back(root, -1);
            while (!q.empty()) {
                auto [s, p] = q.back();
                q.pop_back();
                vs.clear();
                vs.push_back(s);
                fa[s] = -1;
                siz[s] = 1;
                for (int i = 0; i < int(vs.size()); ++i) {
                    int x = vs[i];
                    for (int y : adj[x]) {
                        assert(0 <= y and y < n);
                        if (ban[y] or y == fa[x])
                            continue;
                        assert(vs.size() < adj.size()); // Input must be a forest.
                        fa[y] = x;
                        siz[y] = 1;
                        vs.push_back(y);
                    }
                }
                for (int i = int(vs.size()) - 1; i >= 0; --i) {
                    int x = vs[i];
                    if (fa[x] != -1)
                        siz[fa[x]] += siz[x];
                }
                int rt = s;
                for (;;) {
                    int hv = -1;
                    for (int y : adj[rt])
                        if (!ban[y] and fa[y] == rt and siz[y] > int(vs.size() / 2)) {
                            hv = y;
                            break;
                        }
                    if (hv == -1)
                        break;
                    rt = hv;
                }
                ban[rt] = true;
                dfsOrder.push_back(rt);
                if (cdt != nullptr and p != -1)
                    (*cdt)[p].push_back(rt);
                for (auto it = adj[rt].rbegin(); it != adj[rt].rend(); ++it)
                    if (!ban[*it]) {
                        q.emplace_back(*it, rt);
                    }
            }
        }
    }
};
