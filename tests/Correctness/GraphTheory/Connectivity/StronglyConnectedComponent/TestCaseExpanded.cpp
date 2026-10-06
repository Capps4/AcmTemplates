#include "../../../../Support/TestSupport.hpp"
#include "../../../../../src/GraphTheory/TopSort/code.hpp"
#include "../../../../../src/GraphTheory/Connectivity/StronglyConnectedComponent/code.hpp"
void check(const std::vector<std::vector<int>> &graph) {
    SCC full(graph);
    int n = int(graph.size());
    std::vector<std::vector<bool>> reach(n, std::vector<bool>(n));
    for (int x = 0; x < n; ++x) {
        reach[x][x] = true;
        for (int y : graph[x])
            reach[x][y] = true;
    }
    for (int k = 0; k < n; ++k)
        for (int x = 0; x < n; ++x)
            for (int y = 0; y < n; ++y)
                reach[x][y] = reach[x][y] || (reach[x][k] && reach[k][y]);
    for (int x = 0; x < n; ++x)
        for (int y = 0; y < n; ++y)
            CHECK((full.bel[x] == full.bel[y]) == (reach[x][y] && reach[y][x]));
    std::vector<std::vector<int>> expected(full.cntBlock);
    for (int x = 0; x < n; ++x)
        for (int y : graph[x])
            if (full.bel[x] != full.bel[y]) {
                CHECK(full.bel[x] > full.bel[y]);
                expected[full.bel[x]].push_back(full.bel[y]);
            }
    CHECK(full.g == expected && topSort(full.g).size() == std::size_t(full.cntBlock));
    std::vector<int> ids = full.bel;
    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
    CHECK(ids.size() == std::size_t(full.cntBlock));
}
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("StronglyConnectedComponent/01-empty", [] {
        check({});
    });
    runCase("StronglyConnectedComponent/02-isolates", [] {
        check({{}, {}, {}});
    });
    runCase("StronglyConnectedComponent/03-self-loop", [] {
        check({{0}});
    });
    runCase("StronglyConnectedComponent/04-parallel", [] {
        check({{1, 1}, {}});
    });
    runCase("StronglyConnectedComponent/05-cycle", [] {
        check({{1}, {2}, {0}});
    });
    runCase("StronglyConnectedComponent/06-diamond", [] {
        check({{1, 2}, {3}, {3}, {}});
    });
    runCase("StronglyConnectedComponent/07-two-components", [] {
        check({{1}, {0}, {3}, {2}});
    });
    runCase("StronglyConnectedComponent/08-bridge-components", [] {
        check({{1}, {0, 2}, {3}, {2}});
    });
    runCase("StronglyConnectedComponent/09-incoming-cycle", [] {
        check({{1}, {2}, {1}});
    });
    runCase("StronglyConnectedComponent/10-cycle-tail", [] {
        check({{1}, {2}, {0, 3}, {}});
    });
    return finishCases(10);
}
