#include "Final.hpp"
#include "../TestSupport.hpp"
#include "../GraphTestSupport.hpp"
void checkGraph(const GraphOracle& graph) {
    EdgeBc result(graph.adj);
    auto full = graph.partition();
    CHECK(result.componentNum == full.count);
    std::vector<bool> disabled(graph.edges.size());
    std::vector<int> expectedDegree(graph.adj.size());
    std::vector<std::pair<int, int>> expectedBridges;
    for (int id = 0; id < int(graph.edges.size()); ++id) {
        if (graph.partition(-1, id).count > full.count) {
            disabled[id] = true;
            auto [x, y] = graph.edges[id];
            ++expectedDegree[x]; ++expectedDegree[y];
            expectedBridges.emplace_back(std::min(x, y), std::max(x, y));
        }
    }
    auto parts = graph.partition(-1, -1, disabled);
    CHECK(result.cntBlock == parts.count && result.cutDeg == expectedDegree);
    for (int x = 0; x < int(graph.adj.size()); ++x) for (int y = 0; y < int(graph.adj.size()); ++y)
        CHECK((result.bel[x] == result.bel[y]) == (parts.label[x] == parts.label[y]));
    auto actual = result.bridges;
    for (auto& edge : actual) if (edge.first > edge.second) std::swap(edge.first, edge.second);
    std::sort(actual.begin(), actual.end());
    std::sort(expectedBridges.begin(), expectedBridges.end());
    CHECK(actual == expectedBridges);
    std::vector<std::vector<int>> expected(result.cntBlock);
    for (auto [x, y] : expectedBridges) {
        expected[result.bel[x]].push_back(result.bel[y]);
        expected[result.bel[y]].push_back(result.bel[x]);
    }
    for (int id = 0; id < result.cntBlock; ++id) {
        std::sort(result.g[id].begin(), result.g[id].end());
        std::sort(expected[id].begin(), expected[id].end());
        CHECK(result.g[id] == expected[id]);
    }
}
int main() {
    for (int n = 0; n <= 5; ++n) {
        std::vector<std::pair<int, int>> possible;
        for (int x = 0; x < n; ++x) for (int y = x + 1; y < n; ++y) possible.emplace_back(x, y);
        for (int mask = 0; mask < (1 << possible.size()); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < int(possible.size()); ++i) if (mask >> i & 1) edges.push_back(possible[i]);
            checkGraph(GraphOracle(n, edges));
        }
    }
    for (int trial = 0; trial < 3000; ++trial) {
        int n = randomInt(1, 12);
        std::vector<std::pair<int, int>> edges(randomInt(0, 30));
        for (auto& [x, y] : edges) { x = randomInt(0, n - 1); y = randomInt(0, n - 1); }
        checkGraph(GraphOracle(n, edges));
    }
    const int n = 200000;
    std::vector<std::vector<int>> chain(n);
    for (int x = 1; x < n; ++x) { chain[x - 1].push_back(x); chain[x].push_back(x - 1); }
    EdgeBC path(chain);
    CHECK(path.cntBlock == n && path.bridges.size() == n - 1 && path.componentNum == 1);
    for (int x = 0; x < n; ++x) CHECK(path.cutDeg[x] == (x == 0 || x == n - 1 ? 1 : 2));
    chain[0].push_back(n - 1); chain[n - 1].push_back(0);
    EdgeBc cycle(chain);
    CHECK(cycle.cntBlock == 1 && cycle.bridges.empty());
    auto owned = EdgeBc(std::vector<std::vector<int>>{{1}, {0}});
    CHECK(owned.bridges.size() == 1);
    std::cout << "EdgeBC exhaustive edge-removal oracle, loops/parallel edges, 200K chain/cycle PASS\n";
}
