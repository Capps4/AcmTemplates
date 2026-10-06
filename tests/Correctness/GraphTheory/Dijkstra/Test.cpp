#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/GraphTheory/Dijkstra/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <algorithm>
#include <climits>


int coreCases() {
    using Graph = std::vector<std::vector<std::pair<int, int>>>;
    Graph emptyGraph;
    Dijkstra<int> empty(emptyGraph); // No query is legal on an empty graph.
    for (int trial = 0; trial < 8; ++trial) {
        test_context::step = trial;
        int n = randomInt(1, 30);
        Graph graph(n);
        std::vector<std::vector<long long>> expected(n, std::vector<long long>(n, Dijkstra<int>::Inf));
        for (int i = 0; i < n; ++i) expected[i][i] = 0;
        for (int i = 0; i < n * n / 3; ++i) {
            int x = randomInt(0, n - 1), y = randomInt(0, n - 1), w = randomInt(0, 100);
            graph[x].emplace_back(y, w);
            expected[x][y] = std::min(expected[x][y], 1LL * w);
        }
        for (int k = 0; k < n; ++k) for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j)
            if (expected[i][k] != Dijkstra<int>::Inf && expected[k][j] != Dijkstra<int>::Inf)
                expected[i][j] = std::min(expected[i][j], expected[i][k] + expected[k][j]);
        Dijkstra solver(graph);
        for (int x = 0; x < n; ++x) {
            for (int y = 0; y < n; ++y) CHECK(solver(x, y) == expected[x][y]);
            for (int y = 0; y < n; ++y) CHECK(solver(x, y) == expected[x][y]);
        }
    }
    std::vector<std::vector<std::pair<int, long long>>> large(4);
    large[0].emplace_back(1, 4000000000000000000LL);
    large[1].emplace_back(2, 4000000000000000000LL);
    large[2].emplace_back(3, 4000000000000000000LL);
    Dijkstra<long long> solver(large);
    CHECK(solver(0, 2) == 8000000000000000000LL);
    CHECK(solver(0, 3) == LLONG_MAX);
    std::cout << "300 Floyd-Warshall oracles, CTAD, unreachable/overflow paths and cached source queries passed\n";
    return 0;
}

#include "../../../../src/GraphTheory/Dijkstra/code.hpp"
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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

int run() {
    runCase("Dijkstra/single", [] {
        verifyAdded(1, {});
    });
    runCase("Dijkstra/disconnected", [] {
        verifyAdded(3, {{0, 1, 4}});
    });
    runCase("Dijkstra/zero-cycle", [] {
        verifyAdded(3, {{0, 1, 0}, {1, 0, 0}, {1, 2, 7}});
    });
    runCase("Dijkstra/parallel", [] {
        verifyAdded(2, {{0, 1, 10}, {0, 1, 2}});
    });
    runCase("Dijkstra/stale-heap", [] {
        verifyAdded(3, {{0, 1, 20}, {0, 2, 1}, {2, 1, 1}});
    });
    runCase("Dijkstra/self-loop", [] {
        verifyAdded(2, {{0, 0, 0}, {0, 1, 7}});
    });
    runCase("Dijkstra/equal-paths", [] {
        verifyAdded(4, {{0, 1, 2}, {0, 2, 2}, {1, 3, 3}, {2, 3, 3}});
    });
    runCase("Dijkstra/directed-cycle", [] {
        verifyAdded(3, {{0, 1, 3}, {1, 2, 5}, {2, 0, 1}});
    });
    runCase("Dijkstra/large-weights", [] {
        verifyAdded(3, {{0, 1, 2000000000}, {1, 2, 2000000000}});
    });
    runCase("Dijkstra/many-sources", [] {
        verifyAdded(5, {{0, 1, 4}, {1, 2, 1}, {2, 3, 0}, {3, 4, 2}, {4, 0, 3}, {0, 4, 99}});
    });
    return 0;
}
}

int main() {
    runCase("Dijkstra/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
