#include "../../../../Support/TestStack.hpp"
#include "../../../../../src/GraphTheory/Connectivity/VertexBiconnectedComponent/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../Support/GraphTestSupport.hpp"
#include <set>
void checkGraph(const GraphOracle& graph) {
    int n = int(graph.adj.size());
    VertexBC result(graph.adj);
    auto full = graph.partition();
    CHECK(result.componentNum == full.count);
    std::vector<std::vector<bool>> share(n, std::vector<bool>(n, true));
    for (int x = 0; x < n; ++x) for (int y = x + 1; y < n; ++y)
        share[x][y] = share[y][x] = full.label[x] == full.label[y];
    for (int removed = 0; removed < n; ++removed) {
        auto parts = graph.partition(removed);
        CHECK((result.csqt[removed].size() > 1) == (parts.count > full.count));
        for (int x = 0; x < n; ++x) for (int y = x + 1; y < n; ++y) if (x != removed && y != removed)
            share[x][y] = share[y][x] = share[x][y] && parts.label[x] == parts.label[y];
    }
    std::set<std::vector<int>> expected, actual;
    for (int mask = 1; mask < (1 << n); ++mask) {
        std::vector<int> vertices;
        for (int x = 0; x < n; ++x) if (mask >> x & 1) vertices.push_back(x);
        if (vertices.size() < 2) continue;
        bool clique = true, maximal = true;
        for (int x : vertices) for (int y : vertices) clique &= share[x][y];
        if (!clique) continue;
        for (int x = 0; x < n; ++x) if (!(mask >> x & 1)) {
            bool canAdd = true;
            for (int y : vertices) canAdd &= share[x][y];
            if (canAdd) maximal = false;
        }
        if (maximal) expected.insert(vertices);
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
        for (int block : result.csqt[vertex]) CHECK(n <= block && block < int(result.csqt.size()));
}
int main() {
    return withTestStack([] {
    for (int n = 0; n <= 5; ++n) {
        std::vector<std::pair<int, int>> possible;
        for (int x = 0; x < n; ++x) for (int y = x + 1; y < n; ++y) possible.emplace_back(x, y);
        for (int mask = 0; mask < (1 << possible.size()); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < int(possible.size()); ++i) if (mask >> i & 1) edges.push_back(possible[i]);
            checkGraph(GraphOracle(n, edges));
        }
    }
    for (int trial = 0; trial < 1500; ++trial) {
        int n = randomInt(1, 8);
        std::vector<std::pair<int, int>> edges(randomInt(0, 20));
        for (auto& [x, y] : edges) { x = randomInt(0, n - 1); y = randomInt(0, n - 1); }
        checkGraph(GraphOracle(n, edges));
    }
    const int n = 200000;
    std::vector<std::vector<int>> chain(n);
    for (int x = 1; x < n; ++x) { chain[x - 1].push_back(x); chain[x].push_back(x - 1); }
    VertexBC path(chain);
    CHECK(path.csqt.size() == std::size_t(2 * n - 1) && path.componentNum == 1);
    for (int x = 0; x < n; ++x) CHECK((path.csqt[x].size() > 1) == (x != 0 && x != n - 1));
    chain[0].push_back(n - 1); chain[n - 1].push_back(0);
    VertexBC cycle(chain);
    CHECK(cycle.csqt.size() == std::size_t(n + 1));
    CHECK(cycle.csqt[n].size() == std::size_t(n));
    for (int x = 0; x < n; ++x) CHECK(cycle.csqt[x].size() == 1);
    std::cout << "VertexBC exhaustive vertex-removal/maximal-block oracle, multigraph, 200K chain/cycle PASS\n";
    });
}
