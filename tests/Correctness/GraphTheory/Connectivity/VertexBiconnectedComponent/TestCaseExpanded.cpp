#include "../../../../../src/GraphTheory/Connectivity/VertexBiconnectedComponent/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../Support/GraphTestSupport.hpp"
#include <set>
void checkGraph(const GraphOracle &graph) {
    int n = int(graph.adj.size());
    VertexBC result(graph.adj);
    auto full = graph.partition();
    CHECK(result.componentNum == full.count);
    std::vector<std::vector<bool>> share(n, std::vector<bool>(n, true));
    for (int x = 0; x < n; ++x)
        for (int y = x + 1; y < n; ++y)
            share[x][y] = share[y][x] = full.label[x] == full.label[y];
    for (int removed = 0; removed < n; ++removed) {
        auto parts = graph.partition(removed);
        CHECK((result.csqt[removed].size() > 1) == (parts.count > full.count));
        for (int x = 0; x < n; ++x)
            for (int y = x + 1; y < n; ++y)
                if (x != removed && y != removed)
                    share[x][y] = share[y][x] = share[x][y] && parts.label[x] == parts.label[y];
    }
    std::set<std::vector<int>> expected, actual;
    for (int mask = 1; mask < (1 << n); ++mask) {
        std::vector<int> vertices;
        for (int x = 0; x < n; ++x)
            if (mask >> x & 1)
                vertices.push_back(x);
        if (vertices.size() < 2)
            continue;
        bool clique = true, maximal = true;
        for (int x : vertices)
            for (int y : vertices)
                clique &= share[x][y];
        if (!clique)
            continue;
        for (int x = 0; x < n; ++x)
            if (!(mask >> x & 1)) {
                bool canAdd = true;
                for (int y : vertices)
                    canAdd &= share[x][y];
                if (canAdd)
                    maximal = false;
            }
        if (maximal)
            expected.insert(vertices);
    }
    std::size_t edgeCount = 0;
    for (int block = n; block < int(result.csqt.size()); ++block) {
        auto vertices = result.csqt[block];
        std::sort(vertices.begin(), vertices.end());
        CHECK(std::adjacent_find(vertices.begin(), vertices.end()) == vertices.end());
        for (int vertex : vertices) {
            CHECK(0 <= vertex && vertex < n);
            CHECK(std::count(result.csqt[vertex].begin(), result.csqt[vertex].end(), block) == 1);
        }
        CHECK(actual.insert(vertices).second);
        edgeCount += vertices.size();
    }
    CHECK(actual == expected);
    CHECK(edgeCount + std::size_t(full.count) == result.csqt.size());
    for (int vertex = 0; vertex < n; ++vertex)
        for (int block : result.csqt[vertex])
            CHECK(n <= block && block < int(result.csqt.size()));
}
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("VertexBiconnectedComponent/01-empty", [] {
        checkGraph(GraphOracle(0, {}));
    });
    runCase("VertexBiconnectedComponent/02-isolates", [] {
        checkGraph(GraphOracle(4, {}));
    });
    runCase("VertexBiconnectedComponent/03-self-loop", [] {
        checkGraph(GraphOracle(1, {{0, 0}}));
    });
    runCase("VertexBiconnectedComponent/04-single-edge", [] {
        checkGraph(GraphOracle(2, {{0, 1}}));
    });
    runCase("VertexBiconnectedComponent/05-parallel", [] {
        checkGraph(GraphOracle(2, {{0, 1}, {0, 1}}));
    });
    runCase("VertexBiconnectedComponent/06-triangle", [] {
        checkGraph(GraphOracle(3, {{0, 1}, {1, 2}, {2, 0}}));
    });
    runCase("VertexBiconnectedComponent/07-star", [] {
        checkGraph(GraphOracle(5, {{0, 1}, {0, 2}, {0, 3}, {0, 4}}));
    });
    runCase("VertexBiconnectedComponent/08-two-cycles-cut", [] {
        checkGraph(GraphOracle(5, {{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 2}}));
    });
    runCase("VertexBiconnectedComponent/09-disconnected", [] {
        checkGraph(GraphOracle(5, {{0, 1}, {1, 2}, {2, 0}, {3, 4}}));
    });
    runCase("VertexBiconnectedComponent/10-parallel-tail", [] {
        checkGraph(GraphOracle(4, {{0, 1}, {0, 1}, {1, 2}, {2, 3}, {3, 3}}));
    });
    return finishCases(10);
}
