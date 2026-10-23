#include "../../../../../src/extra/Graph/UndirectedEulerRoad/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace {
using Edges = std::vector<std::pair<int, int>>;

std::string expectedType(int n, const Edges& edges) {
    if (n == 0) {
        return "EulerCircuit";
    }
    int m = int(edges.size());
    auto dfs = [&](auto&& self, int start, int x, int mask) -> int {
        if (mask == (1 << m) - 1) {
            return x == start ? 2 : 1;
        }
        for (int i = 0; i < m; ++i) {
            auto [u, v] = edges[i];
            if (!(mask & (1 << i)) and (u == x or v == x)) {
                int kind = self(self, start, u == x ? v : u, mask | (1 << i));
                if (kind != 0) {
                    return kind;
                }
            }
        }
        return 0;
    };
    for (int x = 0; x < n; ++x) {
        int kind = dfs(dfs, x, x, 0);
        if (kind != 0) {
            return kind == 2 ? "EulerCircuit" : "EulerPath";
        }
    }
    return "NoSolution";
}

void checkGraph(UndirectedEulerRoad& a, const Edges& edges) {
    auto want = expectedType(a.n, edges);
    auto adj = a.adj;
    auto edg = a.edg;
    CHECK(a.work() == want and a.roadType == want);
    auto path = a.ans;
    CHECK(a.work() == want and a.roadType == want and a.ans == path);
    CHECK(a.adj == adj and a.edg == edg);
    if (want == "NoSolution") {
        CHECK(path.empty());
        return;
    }
    CHECK(path.size() == edges.size() + (a.n != 0));
    for (int x : path) {
        CHECK(0 <= x and x < a.n);
    }
    if (!path.empty()) {
        CHECK((path.front() == path.back()) == (want == "EulerCircuit"));
    }
    auto left = edges;
    for (std::size_t i = 1; i < path.size(); ++i) {
        auto it = std::find_if(left.begin(), left.end(), [&](auto edge) {
            return (edge.first == path[i - 1] and edge.second == path[i])
                or (edge.second == path[i - 1] and edge.first == path[i]);
        });
        CHECK(it != left.end());
        left.erase(it);
    }
    CHECK(left.empty());
}

void checkGraph(int n, const Edges& edges) {
    XorGraph g(n);
    for (auto [x, y] : edges) {
        g.add(x, y);
    }
    UndirectedEulerRoad a(g);
    checkGraph(a, edges);
    CHECK(a.adj == g.adj and a.edg == g.edg);
}
} // namespace

int main() {
    runCase("UndirectedEulerRoad/oracle", [] {
        checkGraph(0, {});
        checkGraph(3, {});
        checkGraph(4, {{1, 2}});
        checkGraph(3, {{1, 1}});
        checkGraph(3, {{0, 1}, {0, 1}});
        checkGraph(4, {{0, 1}, {0, 1}, {2, 3}, {2, 3}});
        for (int it = 0; it < 100; ++it) {
            int n = randomInt(1, 5), m = randomInt(0, 7);
            Edges edges;
            for (int i = 0; i < m; ++i) {
                int x = randomInt(0, n - 1), y = randomInt(0, n - 1);
                edges.emplace_back(x, y);
            }
            checkGraph(n, edges);
        }

        // Reuse the same object through circuit, path, and disconnected states.
        UndirectedEulerRoad a(4);
        Edges edges;
        checkGraph(a, edges);
        for (auto edge : Edges{{0, 1}, {0, 1}, {2, 3}, {1, 2}, {1, 3}}) {
            a.add(edge.first, edge.second);
            edges.push_back(edge);
            checkGraph(a, edges);
        }
    });
    return 0;
}
