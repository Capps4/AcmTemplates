#include "../../../../src/GraphTheory/RingTree/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include "../../../Support/GraphTestSupport.hpp"
#include <algorithm>
#include <set>
std::pair<int, int> normalized(int x, int y) {
    return {std::min(x, y), std::max(x, y)};
}
std::vector<int> parents(const RingTree &tree) {
    std::vector<int> p(tree.g.size(), -1);
    for (int x = 0; x < int(tree.g.size()); ++x)
        for (int y : tree.g[x]) {
            CHECK(p[y] == -1);
            p[y] = x;
        }
    return p;
}
void checkUndirected(const GraphOracle &graph) {
    RingTree result(graph.adj);
    auto p = parents(result);
    int n = int(graph.adj.size());
    auto parts = graph.partition();
    std::vector<int> cycleCount(parts.count), vertices(parts.count), edges(parts.count), onCycle(n),
        roots(parts.count);
    std::vector<std::pair<int, int>> represented, original, children, expectedChildren;
    for (int x = 0; x < n; ++x)
        ++vertices[parts.label[x]];
    for (auto [x, y] : graph.edges) {
        ++edges[parts.label[x]];
        original.push_back(normalized(x, y));
    }
    for (const auto &ring : result.rings) {
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
        if (parent == -1)
            ++roots[parts.label[x]];
        else {
            CHECK(0 <= parent && parent < n && !onCycle[x]);
            represented.push_back(normalized(x, parent));
            expectedChildren.emplace_back(parent, x);
        }
        int current = x, steps = 0;
        while (p[current] != -1) {
            current = p[current];
            CHECK(++steps < n);
        }
        for (int child : result.g[x])
            children.emplace_back(x, child);
    }
    for (int component = 0; component < parts.count; ++component) {
        CHECK(cycleCount[component] == edges[component] - vertices[component] + 1);
        if (!cycleCount[component])
            CHECK(roots[component] == 1);
    }
    std::sort(represented.begin(), represented.end());
    std::sort(original.begin(), original.end());
    std::sort(children.begin(), children.end());
    std::sort(expectedChildren.begin(), expectedChildren.end());
    CHECK(represented == original && children == expectedChildren);
}
void checkFunctional(const std::vector<int> &link) {
    RingTree result(link);
    auto p = parents(result);
    int n = int(link.size());
    std::vector<bool> onCycle(n);
    std::set<std::vector<int>> expected, actual;
    for (int start = 0; start < n; ++start) {
        std::vector<bool> seen(n);
        int vertex = start;
        while (!seen[vertex]) {
            seen[vertex] = true;
            vertex = link[vertex];
        }
        if (vertex != start)
            continue;
        onCycle[start] = true;
        std::vector<int> ring;
        do {
            ring.push_back(vertex);
            vertex = link[vertex];
        } while (vertex != start);
        std::sort(ring.begin(), ring.end());
        expected.insert(ring);
    }
    for (auto ring : result.rings) {
        for (std::size_t i = 0; i < ring.size(); ++i)
            CHECK(link[ring[i]] == ring[(i + 1) % ring.size()]);
        std::sort(ring.begin(), ring.end());
        CHECK(actual.insert(ring).second);
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
#include "../../../Support/CaseSupport.hpp"

int main() {
    runCase("RingTree/01-functional-01", [] {
        checkFunctional({});
    });
    runCase("RingTree/02-functional-02", [] {
        checkFunctional({0});
    });
    runCase("RingTree/03-functional-03", [] {
        checkFunctional({1, 0});
    });
    runCase("RingTree/04-functional-04", [] {
        checkFunctional({0, 1});
    });
    runCase("RingTree/05-functional-05", [] {
        checkFunctional({1, 2, 2});
    });
    runCase("RingTree/06-functional-06", [] {
        checkFunctional({1, 2, 0, 2});
    });
    runCase("RingTree/07-functional-07", [] {
        checkFunctional({1, 0, 3, 2});
    });
    runCase("RingTree/08-functional-08", [] {
        checkFunctional({2, 2, 3, 4, 2});
    });
    runCase("RingTree/09-functional-09", [] {
        checkFunctional({0, 0, 0, 0});
    });
    runCase("RingTree/10-functional-10", [] {
        checkFunctional({1, 2, 3, 4, 0});
    });
    return finishCases(10);
}
