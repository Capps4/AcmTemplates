#include "Final.hpp"
#include "../TestSupport.hpp"
#include "../GraphTestSupport.hpp"
#include <algorithm>
#include <set>
std::pair<int, int> normalized(int x, int y) { return {std::min(x, y), std::max(x, y)}; }
std::vector<int> parents(const RingTree& tree) {
    std::vector<int> p(tree.g.size(), -1);
    for (int x = 0; x < int(tree.g.size()); ++x)
        for (int y : tree.g[x]) { CHECK(p[y] == -1); p[y] = x; }
    return p;
}
void checkUndirected(const GraphOracle& graph) {
    RingTree result(graph.adj);
    auto p = parents(result);
    int n = int(graph.adj.size());
    auto parts = graph.partition();
    std::vector<int> cycleCount(parts.count), vertices(parts.count), edges(parts.count), onCycle(n), roots(parts.count);
    std::vector<std::pair<int, int>> represented, original, children, expectedChildren;
    for (int x = 0; x < n; ++x) ++vertices[parts.label[x]];
    for (auto [x, y] : graph.edges) { ++edges[parts.label[x]]; original.push_back(normalized(x, y)); }
    for (const auto& ring : result.rings) {
        CHECK(!ring.empty());
        ++cycleCount[parts.label[ring[0]]];
        for (std::size_t i = 0; i < ring.size(); ++i) {
            int x = ring[i], y = ring[(i + 1) % ring.size()];
            CHECK(0 <= x && x < n && !onCycle[x] && parts.label[x] == parts.label[ring[0]]);
            onCycle[x] = 1;
            CHECK(p[x] == -1);
            represented.push_back(normalized(x, y));
        }
    }
    for (int x = 0; x < n; ++x) {
        int parent = p[x];
        if (parent == -1) ++roots[parts.label[x]];
        else {
            CHECK(0 <= parent && parent < n && !onCycle[x]);
            represented.push_back(normalized(x, parent));
            expectedChildren.emplace_back(parent, x);
        }
        int current = x, steps = 0;
        while (p[current] != -1) { current = p[current]; CHECK(++steps < n); }
        for (int child : result.g[x]) children.emplace_back(x, child);
    }
    for (int component = 0; component < parts.count; ++component) {
        CHECK(cycleCount[component] == edges[component] - vertices[component] + 1);
        if (!cycleCount[component]) CHECK(roots[component] == 1);
    }
    std::sort(represented.begin(), represented.end()); std::sort(original.begin(), original.end());
    std::sort(children.begin(), children.end()); std::sort(expectedChildren.begin(), expectedChildren.end());
    CHECK(represented == original && children == expectedChildren);
}
void checkFunctional(const std::vector<int>& link) {
    RingTree result(link);
    auto p = parents(result);
    int n = int(link.size());
    std::vector<bool> onCycle(n);
    std::set<std::vector<int>> expected, actual;
    for (int start = 0; start < n; ++start) {
        std::vector<bool> seen(n);
        int vertex = start;
        while (!seen[vertex]) { seen[vertex] = true; vertex = link[vertex]; }
        if (vertex != start) continue;
        onCycle[start] = true;
        std::vector<int> ring;
        do { ring.push_back(vertex); vertex = link[vertex]; } while (vertex != start);
        std::sort(ring.begin(), ring.end()); expected.insert(ring);
    }
    for (auto ring : result.rings) {
        for (std::size_t i = 0; i < ring.size(); ++i) CHECK(link[ring[i]] == ring[(i + 1) % ring.size()]);
        std::sort(ring.begin(), ring.end()); CHECK(actual.insert(ring).second);
    }
    CHECK(actual == expected);
    std::vector<std::pair<int, int>> edges;
    for (int vertex = 0; vertex < n; ++vertex) {
        CHECK(p[vertex] == (onCycle[vertex] ? -1 : link[vertex]));
        edges.emplace_back(vertex, link[vertex]);
    }
    GraphOracle graph(n, edges);
    checkUndirected(graph);
    RingTree undirected(graph.adj);
    CHECK(parents(undirected) == parents(result));
}
int main() {
    for (int n = 0; n <= 5; ++n) {
        int combinations = 1;
        for (int i = 0; i < n; ++i) combinations *= n;
        for (int code = 0; code < combinations; ++code) {
            std::vector<int> link(n);
            int value = code;
            for (auto& next : link) { next = value % n; value /= n; }
            checkFunctional(link);
        }
    }
    checkUndirected(GraphOracle(2, {{0, 1}})); // Old code drops this edge.
    for (int trial = 0; trial < 1000; ++trial) {
        int n = randomInt(0, 30);
        std::vector<std::pair<int, int>> edges;
        for (int x = 1; x < n; ++x) if (randomInt(0, 4)) edges.emplace_back(x, randomInt(0, x - 1));
        checkUndirected(GraphOracle(n, edges));
    }
    const int n = 200000;
    std::vector<int> link(n);
    for (int x = 0; x < n; ++x) link[x] = std::min(x + 1, n - 1);
    RingTree deep(link);
    CHECK(deep.rings == std::vector<std::vector<int>>{{n - 1}});
    auto dp = parents(deep);
    for (int x = 0; x + 1 < n; ++x) CHECK(dp[x] == x + 1 && deep.g[x + 1] == std::vector<int>{x});
    std::vector<std::vector<int>> chain(n);
    for (int x = 1; x < n; ++x) { chain[x].push_back(x - 1); chain[x - 1].push_back(x); }
    RingTree tree(chain);
    auto p = parents(tree);
    CHECK(tree.rings.empty() && std::count(p.begin(), p.end(), -1) == 1);
    std::size_t edgeCount = 0; for (const auto& row : tree.g) edgeCount += row.size();
    CHECK(edgeCount == n - 1);
    std::cout << "RingTree exhaustive functional graph + physical-edge conservation oracle, forests/loops/parallel, 200K chain PASS\n";
}
