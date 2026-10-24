#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/extra/Tree/CentroidDecomposition/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <algorithm>
#include <numeric>

std::vector<int> component(const std::vector<std::vector<int>>& adj, int start,
                           const std::vector<bool>& excluded) {
    std::vector<bool> seen = excluded;
    std::vector<int> queue{start};
    seen[start] = true;
    for (std::size_t head = 0; head < queue.size(); ++head)
        for (int next : adj[queue[head]]) if (!seen[next]) { seen[next] = true; queue.push_back(next); }
    return queue;
}
void check(const std::vector<std::vector<int>>& adj) {
    std::vector<std::vector<int>> cdt{{-1}};
    CentroidDecomposition tree(adj, &cdt);
    CentroidDecomposition plain(adj);
    CHECK(tree.dfsOrder == plain.dfsOrder);
    CHECK(cdt.size() == adj.size());
    std::vector<int> parent(adj.size(), -1), want(adj.size(), -1);
    for (int x = 0; x < int(cdt.size()); ++x)
        for (int y : cdt[x]) {
            CHECK(0 <= y and y < int(adj.size()) and parent[y] == -1);
            parent[y] = x;
        }
    auto sorted = tree.dfsOrder;
    std::sort(sorted.begin(), sorted.end());
    std::vector<int> all(adj.size()); std::iota(all.begin(), all.end(), 0);
    CHECK(sorted == all);
    std::vector<bool> excluded(adj.size());
    for (int x : tree.dfsOrder) {
        auto part = component(adj, x, excluded);
        CHECK(parent[x] == want[x]);
        for (int y : part)
            if (y != x)
                want[y] = x;
        excluded[x] = true;
        for (int y : adj[x]) if (!excluded[y])
            CHECK(component(adj, y, excluded).size() <= part.size() / 2);
    }

}
int coreCases() {
    check({}); check({{}}); check({{1}, {0}});
    for (int trial = 0; trial < 8; ++trial) {
        test_context::step = trial;
        int n = randomInt(0, 80);
        std::vector<std::vector<int>> adj(n);
        for (int x = 1; x < n; ++x) if (randomInt(0, 4)) {
            int y = randomInt(0, x - 1);
            adj[x].push_back(y); adj[y].push_back(x);
        }
        check(adj);
    }
    const int n = 128;
    std::vector<std::vector<int>> chain(n);
    for (int x = 1; x < n; ++x) { chain[x].push_back(x - 1); chain[x - 1].push_back(x); }
    CentroidDecomposition longChain(chain);
    CHECK(longChain.dfsOrder.size() == std::size_t(n));
    CHECK(longChain.dfsOrder[0] == n / 2 - 1);
    auto owned = CentroidDecomposition(std::vector<std::vector<int>>{{1}, {0}});
    CHECK(owned.dfsOrder.size() == 2);
    std::cout << "CentroidDecomposition independent component balance oracle, forests/empty, 128-node chain PASS\n";
    return 0;
}

#include "../../../../../src/extra/Tree/CentroidDecomposition/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <algorithm>
#include <numeric>
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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
    std::vector<std::vector<int>> cdt{{-1}};
    CentroidDecomposition tree(adj, &cdt);
    CentroidDecomposition plain(adj);
    CHECK(tree.dfsOrder == plain.dfsOrder);
    CHECK(cdt.size() == adj.size());
    std::vector<int> parent(adj.size(), -1), want(adj.size(), -1);
    for (int x = 0; x < int(cdt.size()); ++x)
        for (int y : cdt[x]) {
            CHECK(0 <= y and y < int(adj.size()) and parent[y] == -1);
            parent[y] = x;
        }
    auto sorted = tree.dfsOrder;
    std::sort(sorted.begin(), sorted.end());
    std::vector<int> all(adj.size());
    std::iota(all.begin(), all.end(), 0);
    CHECK(sorted == all);
    std::vector<bool> excluded(adj.size());
    for (int x : tree.dfsOrder) {
        auto part = component(adj, x, excluded);
        CHECK(parent[x] == want[x]);
        for (int y : part)
            if (y != x)
                want[y] = x;
        excluded[x] = true;
        for (int y : adj[x])
            if (!excluded[y])
                CHECK(component(adj, y, excluded).size() <= part.size() / 2);
    }
}

int run() {
    runCase("CentroidDecomposition/empty", [] {
        check({});
    });
    runCase("CentroidDecomposition/single", [] {
        check({{}});
    });
    runCase("CentroidDecomposition/edge", [] {
        check({{1}, {0}});
    });
    runCase("CentroidDecomposition/chain", [] {
        check({{1}, {0, 2}, {1, 3}, {2, 4}, {3}});
    });
    runCase("CentroidDecomposition/star", [] {
        check({{1, 2, 3, 4}, {0}, {0}, {0}, {0}});
    });
    runCase("CentroidDecomposition/balanced", [] {
        check({{1, 2}, {0, 3, 4}, {0, 5, 6}, {1}, {1}, {2}, {2}});
    });
    runCase("CentroidDecomposition/unequal-branches", [] {
        check({{1, 3}, {0, 2}, {1}, {0, 4}, {3, 5}, {4}});
    });
    runCase("CentroidDecomposition/reversed-neighbors", [] {
        check({{2, 1}, {4, 3, 0}, {6, 5, 0}, {1}, {1}, {2}, {2}});
    });
    runCase("CentroidDecomposition/broom", [] {
        check({{1}, {0, 2}, {1, 3, 4, 5}, {2}, {2}, {2}});
    });
    runCase("CentroidDecomposition/two-branches", [] {
        check({{1, 4}, {0, 2}, {1, 3}, {2}, {0, 5}, {4}});
    });
    return 0;
}
}

int main() {
    runCase("CentroidDecomposition/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    runCase("CentroidDecomposition/deep-chain", [] {
        const int n = 20000;
        std::vector<std::vector<int>> g(n), cdt;
        for (int x = 1; x < n; ++x) {
            g[x].push_back(x - 1);
            g[x - 1].push_back(x);
        }
        CentroidDecomposition a(g, &cdt);
        CHECK(a.dfsOrder.size() == std::size_t(n));
        CHECK(a.dfsOrder[0] == n / 2 - 1);
        std::vector<int> dep(n);
        int cnt = 0;
        for (int x : a.dfsOrder)
            for (int y : cdt[x]) {
                CHECK(dep[y] == 0);
                dep[y] = dep[x] + 1;
                CHECK(dep[y] <= 15);
                ++cnt;
            }
        CHECK(cnt == n - 1);
    });
    return 0;
}
