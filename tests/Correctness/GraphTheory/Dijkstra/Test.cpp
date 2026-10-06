#include "../../../../src/GraphTheory/Dijkstra/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <algorithm>
#include <climits>

int main() {
    using Graph = std::vector<std::vector<std::pair<int, int>>>;
    Graph emptyGraph;
    Dijkstra<int> empty(emptyGraph); // No query is legal on an empty graph.
    for (int trial = 0; trial < 300; ++trial) {
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
}
