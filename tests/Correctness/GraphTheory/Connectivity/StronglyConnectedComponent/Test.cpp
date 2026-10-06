#include "../../../../Support/TestStack.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../../src/GraphTheory/TopSort/code.hpp"
#include "../../../../../src/GraphTheory/Connectivity/StronglyConnectedComponent/code.hpp"
void check(const std::vector<std::vector<int>>& graph) {
    SCC full(graph);
    int n = int(graph.size());
    std::vector<std::vector<bool>> reach(n, std::vector<bool>(n));
    for (int x = 0; x < n; ++x) {
        reach[x][x] = true;
        for (int y : graph[x]) reach[x][y] = true;
    }
    for (int k = 0; k < n; ++k) for (int x = 0; x < n; ++x)
        for (int y = 0; y < n; ++y) reach[x][y] = reach[x][y] || (reach[x][k] && reach[k][y]);
    for (int x = 0; x < n; ++x) for (int y = 0; y < n; ++y)
        CHECK((full.bel[x] == full.bel[y]) == (reach[x][y] && reach[y][x]));
    std::vector<std::vector<int>> expected(full.cntBlock);
    for (int x = 0; x < n; ++x) for (int y : graph[x]) if (full.bel[x] != full.bel[y]) {
        CHECK(full.bel[x] > full.bel[y]);
        expected[full.bel[x]].push_back(full.bel[y]);
    }
    CHECK(full.g == expected && topSort(full.g).size() == std::size_t(full.cntBlock));
    std::vector<int> ids = full.bel;
    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
    CHECK(ids.size() == std::size_t(full.cntBlock));
}
int main() {
    return withTestStack([] {
    check({});
    for (int n = 1; n <= 3; ++n) for (int mask = 0; mask < (1 << (n * n)); ++mask) {
        std::vector<std::vector<int>> graph(n);
        for (int x = 0; x < n; ++x) for (int y = 0; y < n; ++y)
            if (mask >> (x * n + y) & 1) graph[x].push_back(y);
        check(graph);
    }
    for (int iteration = 0; iteration < 2000; ++iteration) {
        int n = randomInt(1, 20);
        std::vector<std::vector<int>> graph(n);
        for (int edge = 0, m = randomInt(0, 100); edge < m; ++edge)
            graph[randomInt(0, n - 1)].push_back(randomInt(0, n - 1));
        check(graph);
    }
    const int n = 200000;
    std::vector<std::vector<int>> graph(n);
    for (int x = 0; x + 1 < n; ++x) graph[x].push_back(x + 1);
    SCC chain(graph);
    CHECK(chain.cntBlock == n);
    for (int x = 0; x < n; ++x) CHECK(chain.bel[x] == n - x - 1);
    graph.back().push_back(0);
    SCC cycle(graph);
    CHECK(cycle.cntBlock == 1 && cycle.g.size() == 1 && cycle.g[0].empty());
    auto ownedResult = SCC(std::vector<std::vector<int>>{{1}, {0}});
    CHECK(ownedResult.bel == std::vector<int>({0, 0}));
    std::cout << "SCC exhaustive/Floyd oracle, condensation integration, 200K chain/cycle PASS\n";
    });
}
