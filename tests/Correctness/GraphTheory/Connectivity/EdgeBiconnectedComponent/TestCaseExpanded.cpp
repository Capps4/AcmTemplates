#include "../../../../../src/GraphTheory/Connectivity/EdgeBiconnectedComponent/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../Support/GraphTestSupport.hpp"
void checkGraph(const GraphOracle &graph) {
    EdgeBC result(graph.adj);
    auto full = graph.partition();
    CHECK(result.componentNum == full.count);
    std::vector<bool> disabled(graph.edges.size());
    std::vector<int> expectedDegree(graph.adj.size());
    std::vector<std::pair<int, int>> expectedBridges;
    for (int id = 0; id < int(graph.edges.size()); ++id) {
        if (graph.partition(-1, id).count > full.count) {
            disabled[id] = true;
            auto [x, y] = graph.edges[id];
            ++expectedDegree[x];
            ++expectedDegree[y];
            expectedBridges.emplace_back(std::min(x, y), std::max(x, y));
        }
    }
    auto parts = graph.partition(-1, -1, disabled);
    CHECK(result.cntBlock == parts.count && result.cutDeg == expectedDegree);
    for (int x = 0; x < int(graph.adj.size()); ++x)
        for (int y = 0; y < int(graph.adj.size()); ++y)
            CHECK((result.bel[x] == result.bel[y]) == (parts.label[x] == parts.label[y]));
    std::vector<std::pair<int, int>> actual;
    for (auto [x, y] : graph.edges)
        if (result.bel[x] != result.bel[y])
            actual.emplace_back(x, y);
    for (auto &edge : actual)
        if (edge.first > edge.second)
            std::swap(edge.first, edge.second);
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
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("EdgeBiconnectedComponent/01-empty", [] {
        checkGraph(GraphOracle(0, {}));
    });
    runCase("EdgeBiconnectedComponent/02-isolates", [] {
        checkGraph(GraphOracle(4, {}));
    });
    runCase("EdgeBiconnectedComponent/03-self-loop", [] {
        checkGraph(GraphOracle(1, {{0, 0}}));
    });
    runCase("EdgeBiconnectedComponent/04-single-edge", [] {
        checkGraph(GraphOracle(2, {{0, 1}}));
    });
    runCase("EdgeBiconnectedComponent/05-parallel", [] {
        checkGraph(GraphOracle(2, {{0, 1}, {0, 1}}));
    });
    runCase("EdgeBiconnectedComponent/06-triangle", [] {
        checkGraph(GraphOracle(3, {{0, 1}, {1, 2}, {2, 0}}));
    });
    runCase("EdgeBiconnectedComponent/07-star", [] {
        checkGraph(GraphOracle(5, {{0, 1}, {0, 2}, {0, 3}, {0, 4}}));
    });
    runCase("EdgeBiconnectedComponent/08-two-cycles-cut", [] {
        checkGraph(GraphOracle(5, {{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 2}}));
    });
    runCase("EdgeBiconnectedComponent/09-disconnected", [] {
        checkGraph(GraphOracle(5, {{0, 1}, {1, 2}, {2, 0}, {3, 4}}));
    });
    runCase("EdgeBiconnectedComponent/10-parallel-tail", [] {
        checkGraph(GraphOracle(4, {{0, 1}, {0, 1}, {1, 2}, {2, 3}, {3, 3}}));
    });
    return finishCases(10);
}
