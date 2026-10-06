#include <memory>
#include "../../../Support/TestSupport.hpp"
#include "../../../../src/GraphTheory/TopSort/code.hpp"
void check(const std::vector<std::vector<int>> &graph) {
    int n = int(graph.size());
    auto order = topSort(graph);
    std::vector<int> rank(n, -1), degree(n);
    for (int i = 0; i < int(order.size()); ++i) {
        CHECK(0 <= order[i] && order[i] < n && rank[order[i]] == -1);
        rank[order[i]] = i;
    }
    for (int x = 0; x < n; ++x)
        for (int y : graph[x])
            ++degree[y];
    for (int x : order) {
        CHECK(degree[x] == 0);
        for (int y : graph[x])
            --degree[y];
    }
    for (int x = 0; x < n; ++x)
        if (rank[x] == -1)
            CHECK(degree[x] > 0);
    std::vector<std::vector<bool>> reach(n, std::vector<bool>(n));
    for (int x = 0; x < n; ++x)
        for (int y : graph[x])
            reach[x][y] = true;
    for (int k = 0; k < n; ++k)
        for (int x = 0; x < n; ++x)
            for (int y = 0; y < n; ++y)
                reach[x][y] = reach[x][y] || (reach[x][k] && reach[k][y]);
    bool cyclic = false;
    for (int x = 0; x < n; ++x)
        cyclic |= reach[x][x];
    CHECK((order.size() == graph.size()) == !cyclic);
}
#include "../../../Support/CaseSupport.hpp"

int main() {
    runCase("TopSort/01-empty", [] {
        check({});
    });
    runCase("TopSort/02-isolates", [] {
        check({{}, {}, {}});
    });
    runCase("TopSort/03-self-loop", [] {
        check({{0}});
    });
    runCase("TopSort/04-parallel", [] {
        check({{1, 1}, {}});
    });
    runCase("TopSort/05-cycle", [] {
        check({{1}, {2}, {0}});
    });
    runCase("TopSort/06-diamond", [] {
        check({{1, 2}, {3}, {3}, {}});
    });
    runCase("TopSort/07-two-components", [] {
        check({{1}, {0}, {3}, {2}});
    });
    runCase("TopSort/08-bridge-components", [] {
        check({{1}, {0, 2}, {3}, {2}});
    });
    runCase("TopSort/09-incoming-cycle", [] {
        check({{1}, {2}, {1}});
    });
    runCase("TopSort/10-cycle-tail", [] {
        check({{1}, {2}, {0, 3}, {}});
    });
    return finishCases(10);
}
