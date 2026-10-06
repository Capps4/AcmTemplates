#pragma once
#include <utility>
#include <vector>

// Slow independent reachability oracle with physical edge IDs for multigraphs.
struct GraphOracle {
    struct Partition { std::vector<int> label; int count = 0; };
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<int>> adj, edgeIds;
    GraphOracle(int n, std::vector<std::pair<int, int>> input)
        : edges(std::move(input)), adj(n), edgeIds(n) {
        for (int id = 0; id < int(edges.size()); ++id) {
            auto [x, y] = edges[id];
            adj[x].push_back(y); adj[y].push_back(x);
            edgeIds[x].push_back(id); edgeIds[y].push_back(id);
        }
    }
    Partition partition(int skippedVertex = -1, int skippedEdge = -1,
                        const std::vector<bool>& disabled = {}) const {
        Partition result{std::vector<int>(adj.size(), -1)};
        for (int root = 0; root < int(adj.size()); ++root) {
            if (root == skippedVertex || result.label[root] != -1) continue;
            std::vector<int> queue{root};
            result.label[root] = result.count;
            for (std::size_t head = 0; head < queue.size(); ++head) {
                int vertex = queue[head];
                for (int id : edgeIds[vertex]) {
                    if (id == skippedEdge || (!disabled.empty() && disabled[id])) continue;
                    auto [x, y] = edges[id];
                    int next = x == vertex ? y : x;
                    if (next == skippedVertex || result.label[next] != -1) continue;
                    result.label[next] = result.count;
                    queue.push_back(next);
                }
            }
            ++result.count;
        }
        return result;
    }
};
