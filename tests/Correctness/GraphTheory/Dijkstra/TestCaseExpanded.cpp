#include "../../../../src/GraphTheory/Dijkstra/code.hpp"
#include "../../../Support/CaseSupport.hpp"
void verifyAdded(int n, const std::vector<std::tuple<int, int, int>> &edges) {
    std::vector<std::vector<std::pair<int, int>>> g(n);
    std::vector<std::vector<long long>> d(n, std::vector<long long>(n, Dijkstra<int>::Inf));
    for (int i = 0; i < n; ++i)
        d[i][i] = 0;
    for (auto [x, y, w] : edges) {
        g[x].push_back({y, w});
        d[x][y] = std::min(d[x][y], 1LL * w);
    }
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (d[i][k] != Dijkstra<int>::Inf and d[k][j] != Dijkstra<int>::Inf)
                    d[i][j] = std::min(d[i][j], d[i][k] + d[k][j]);
    Dijkstra<int> solver(g);
    for (int i = n - 1; i >= 0; --i)
        for (int j = 0; j < n; ++j) {
            CHECK(solver(i, j) == d[i][j]);
            CHECK(solver(i, j) == d[i][j]);
        }
}

int main() {
    runCase("Dijkstra/01-single", [] {
        verifyAdded(1, {});
    });
    runCase("Dijkstra/02-disconnected", [] {
        verifyAdded(3, {{0, 1, 4}});
    });
    runCase("Dijkstra/03-zero-cycle", [] {
        verifyAdded(3, {{0, 1, 0}, {1, 0, 0}, {1, 2, 7}});
    });
    runCase("Dijkstra/04-parallel", [] {
        verifyAdded(2, {{0, 1, 10}, {0, 1, 2}});
    });
    runCase("Dijkstra/05-stale-heap", [] {
        verifyAdded(3, {{0, 1, 20}, {0, 2, 1}, {2, 1, 1}});
    });
    runCase("Dijkstra/06-self-loop", [] {
        verifyAdded(2, {{0, 0, 0}, {0, 1, 7}});
    });
    runCase("Dijkstra/07-equal-paths", [] {
        verifyAdded(4, {{0, 1, 2}, {0, 2, 2}, {1, 3, 3}, {2, 3, 3}});
    });
    runCase("Dijkstra/08-directed-cycle", [] {
        verifyAdded(3, {{0, 1, 3}, {1, 2, 5}, {2, 0, 1}});
    });
    runCase("Dijkstra/09-large-weights", [] {
        verifyAdded(3, {{0, 1, 2000000000}, {1, 2, 2000000000}});
    });
    runCase("Dijkstra/10-many-sources", [] {
        verifyAdded(5, {{0, 1, 4}, {1, 2, 1}, {2, 3, 0}, {3, 4, 2}, {4, 0, 3}, {0, 4, 99}});
    });
    return finishCases(10);
}
