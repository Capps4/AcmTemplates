#pragma once
#include "../../../../Headers/Headers.hpp"

struct BiGraphColor {
    std::vector<int> col;
    bool isBiGraph = true;

    explicit BiGraphColor(const std::vector<std::vector<int>>& adj) : col(adj.size(), -1) {
        for (int r = 0; r < int(adj.size()); ++r) {
            if (col[r] != -1) {
                continue;
            }
            col[r] = 0;
            std::vector<int> q{r};
            for (std::size_t i = 0; i < q.size(); ++i) {
                int x = q[i];
                for (int y : adj[x]) {
                    assert(y >= 0 and y < int(adj.size()));
                    if (col[y] == -1) {
                        col[y] = col[x] ^ 1;
                        q.push_back(y);
                    } else if (col[y] == col[x]) {
                        isBiGraph = false;
                        return;
                    }
                }
            }
        }
    }
};
