#include "../../../Support/CaseSupport.hpp"
#include <memory>
#include "../../../Support/TestSupport.hpp"
#include "../../../../src/GraphTheory/TopSort/code.hpp"

void check(const std::vector<std::vector<int>>& graph) {
    int n = int(graph.size());
    auto order = topSort(graph);
    std::vector<int> rank(n, -1), degree(n);
    for (int i = 0; i < int(order.size()); ++i) {
        CHECK(0 <= order[i] && order[i] < n && rank[order[i]] == -1);
        rank[order[i]] = i;
    }
    for (int x = 0; x < n; ++x) for (int y : graph[x]) ++degree[y];
    for (int x : order) {
        CHECK(degree[x] == 0);
        for (int y : graph[x]) --degree[y];
    }
    for (int x = 0; x < n; ++x) if (rank[x] == -1) CHECK(degree[x] > 0);
    std::vector<std::vector<bool>> reach(n, std::vector<bool>(n));
    for (int x = 0; x < n; ++x) for (int y : graph[x]) reach[x][y] = true;
    for (int k = 0; k < n; ++k) for (int x = 0; x < n; ++x)
        for (int y = 0; y < n; ++y) reach[x][y] = reach[x][y] || (reach[x][k] && reach[k][y]);
    bool cyclic = false;
    for (int x = 0; x < n; ++x) cyclic |= reach[x][x];
    CHECK((order.size() == graph.size()) == !cyclic);
}
int coreCases() {
    check({});
    for (int n = 1; n <= 3; ++n) for (int mask = 0; mask < (1 << (n * n)); ++mask) {
        std::vector<std::vector<int>> graph(n);
        for (int x = 0; x < n; ++x) for (int y = 0; y < n; ++y)
            if (mask >> (x * n + y) & 1) graph[x].push_back(y);
        check(graph);
    }
    for (int iteration = 0; iteration < 16; ++iteration) {
        test_context::step = iteration;
        int n = randomInt(1, 20);
        std::vector<std::vector<int>> graph(n);
        for (int edge = 0, m = randomInt(0, 100); edge < m; ++edge)
            graph[randomInt(0, n - 1)].push_back(randomInt(0, n - 1));
        check(graph);
    }
    std::vector<std::vector<std::pair<int, std::unique_ptr<int>>>> weighted(3);
    weighted[0].emplace_back(1, std::make_unique<int>(7));
    weighted[1].emplace_back(2, std::make_unique<int>(8));
    CHECK(topSort(weighted) == std::vector<int>({0, 1, 2}));
    CHECK(*weighted[0][0].second == 7);
    std::vector<std::vector<int>> longChain(128);
    for (int x = 0; x + 1 < int(longChain.size()); ++x) longChain[x].push_back(x + 1);
    auto order = topSort(longChain);
    CHECK(order.size() == longChain.size() && order.front() == 0 && order.back() == 127);
    std::cout << "TopSort exhaustive/Floyd oracle, multigraph, move-only edges, 200K chain PASS\n";
    return 0;
}

#include <memory>
#include "../../../Support/TestSupport.hpp"
#include "../../../../src/GraphTheory/TopSort/code.hpp"
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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

int run() {
    runCase("TopSort/empty", [] {
        check({});
    });
    runCase("TopSort/isolates", [] {
        check({{}, {}, {}});
    });
    runCase("TopSort/self-loop", [] {
        check({{0}});
    });
    runCase("TopSort/parallel", [] {
        check({{1, 1}, {}});
    });
    runCase("TopSort/cycle", [] {
        check({{1}, {2}, {0}});
    });
    runCase("TopSort/diamond", [] {
        check({{1, 2}, {3}, {3}, {}});
    });
    runCase("TopSort/two-components", [] {
        check({{1}, {0}, {3}, {2}});
    });
    runCase("TopSort/bridge-components", [] {
        check({{1}, {0, 2}, {3}, {2}});
    });
    runCase("TopSort/incoming-cycle", [] {
        check({{1}, {2}, {1}});
    });
    runCase("TopSort/cycle-tail", [] {
        check({{1}, {2}, {0, 3}, {}});
    });
    return 0;
}
}

int main() {
    runCase("TopSort/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
