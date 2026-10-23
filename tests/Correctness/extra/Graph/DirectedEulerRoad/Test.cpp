#include "../../../../../src/extra/Graph/DirectedEulerRoad/code.hpp"
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
            if (!(mask & (1 << i)) and edges[i].first == x) {
                int kind = self(self, start, edges[i].second, mask | (1 << i));
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

void checkGraph(int n, const Edges& edges) {
    std::vector<std::vector<int>> g(n);
    for (auto [x, y] : edges) {
        g[x].push_back(y);
    }
    auto before = g;
    auto want = expectedType(n, edges);
    DirectedEulerRoad a(g);
    CHECK(g == before and a.roadType == want);
    if (want == "NoSolution") {
        CHECK(a.ans.empty());
        return;
    }
    CHECK(a.ans.size() == edges.size() + (n != 0));
    for (int x : a.ans) {
        CHECK(0 <= x and x < n);
    }
    if (!a.ans.empty()) {
        CHECK((a.ans.front() == a.ans.back()) == (want == "EulerCircuit"));
    }
    auto left = edges;
    for (std::size_t i = 1; i < a.ans.size(); ++i) {
        auto edge = std::make_pair(a.ans[i - 1], a.ans[i]);
        auto it = std::find(left.begin(), left.end(), edge);
        CHECK(it != left.end());
        left.erase(it);
    }
    CHECK(left.empty());
}
} // namespace

int main() {
    runCase("DirectedEulerRoad/oracle", [] {
        checkGraph(0, {});
        checkGraph(3, {});
        checkGraph(4, {{1, 2}});
        checkGraph(3, {{1, 1}});
        checkGraph(3, {{0, 1}, {0, 1}, {1, 0}});
        checkGraph(4, {{0, 1}, {1, 0}, {2, 3}, {3, 2}});
        for (int it = 0; it < 100; ++it) {
            int n = randomInt(1, 5), m = randomInt(0, 7);
            Edges edges;
            for (int i = 0; i < m; ++i) {
                int x = randomInt(0, n - 1), y = randomInt(0, n - 1);
                edges.emplace_back(x, y);
            }
            checkGraph(n, edges);
        }
    });
    return 0;
}
