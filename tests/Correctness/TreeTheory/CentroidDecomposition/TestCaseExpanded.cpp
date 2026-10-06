#include "../../../../src/TreeTheory/CentroidDecomposition/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <algorithm>
#include <numeric>
std::vector<int> component(const std::vector<std::vector<int>> &adj, int start,
                           const std::vector<bool> &excluded) {
    std::vector<bool> seen = excluded;
    std::vector<int> queue{start};
    seen[start] = true;
    for (std::size_t head = 0; head < queue.size(); ++head)
        for (int next : adj[queue[head]])
            if (!seen[next]) {
                seen[next] = true;
                queue.push_back(next);
            }
    return queue;
}
void check(const std::vector<std::vector<int>> &adj) {
    CentroidDecomposition tree(adj);
    auto sorted = tree.dfsOrder;
    std::sort(sorted.begin(), sorted.end());
    std::vector<int> all(adj.size());
    std::iota(all.begin(), all.end(), 0);
    CHECK(sorted == all);
    std::vector<bool> excluded(adj.size());
    for (int x : tree.dfsOrder) {
        auto part = component(adj, x, excluded);
        excluded[x] = true;
        for (int y : adj[x])
            if (!excluded[y])
                CHECK(component(adj, y, excluded).size() <= part.size() / 2);
    }
}
#include "../../../Support/CaseSupport.hpp"

int main() {
    runCase("CentroidDecomposition/01-empty", [] {
        check({});
    });
    runCase("CentroidDecomposition/02-single", [] {
        check({{}});
    });
    runCase("CentroidDecomposition/03-edge", [] {
        check({{1}, {0}});
    });
    runCase("CentroidDecomposition/04-chain", [] {
        check({{1}, {0, 2}, {1, 3}, {2, 4}, {3}});
    });
    runCase("CentroidDecomposition/05-star", [] {
        check({{1, 2, 3, 4}, {0}, {0}, {0}, {0}});
    });
    runCase("CentroidDecomposition/06-balanced", [] {
        check({{1, 2}, {0, 3, 4}, {0, 5, 6}, {1}, {1}, {2}, {2}});
    });
    runCase("CentroidDecomposition/07-unequal-branches", [] {
        check({{1, 3}, {0, 2}, {1}, {0, 4}, {3, 5}, {4}});
    });
    runCase("CentroidDecomposition/08-reversed-neighbors", [] {
        check({{2, 1}, {4, 3, 0}, {6, 5, 0}, {1}, {1}, {2}, {2}});
    });
    runCase("CentroidDecomposition/09-broom", [] {
        check({{1}, {0, 2}, {1, 3, 4, 5}, {2}, {2}, {2}});
    });
    runCase("CentroidDecomposition/10-two-branches", [] {
        check({{1, 4}, {0, 2}, {1, 3}, {2}, {0, 5}, {4}});
    });
    return finishCases(10);
}
