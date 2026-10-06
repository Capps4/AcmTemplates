#include "../../../../src/TreeTheory/Tree/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <memory>
#include <set>
int ancestor(int x, int y, const std::vector<int> &parent, const std::vector<int> &depth) {
    while (depth[x] > depth[y])
        x = parent[x];
    while (depth[y] > depth[x])
        y = parent[y];
    while (x != y) {
        x = parent[x];
        y = parent[y];
    }
    return x;
}
bool isAncestor(int x, int y, const std::vector<int> &parent) {
    while (y != -1) {
        if (x == y)
            return true;
        y = parent[y];
    }
    return false;
}
void checkVirtual(const FullTree<int> &tree, const std::vector<int> &keys,
                  const std::vector<int> &extra, const std::vector<int> &parent,
                  const std::vector<int> &depth) {
    std::set<int> closure(keys.begin(), keys.end());
    closure.insert(extra.begin(), extra.end());
    auto initial = closure;
    for (int x : initial)
        for (int y : initial)
            closure.insert(ancestor(x, y, parent, depth));
    std::vector<int> expected(closure.begin(), closure.end());
    std::sort(expected.begin(), expected.end(), [&](int x, int y) {
        return tree.dfn[x] < tree.dfn[y];
    });
    auto result = tree.getVirtualTree(keys, extra);
    CHECK(result.dfsOrder == expected);
    auto adjacency = result.adj();
    for (int i = 0; i < int(expected.size()); ++i) {
        int closest = -1;
        for (int j = 0; j < i; ++j)
            if (isAncestor(expected[j], expected[i], parent) &&
                (closest == -1 || depth[expected[j]] > depth[expected[closest]]))
                closest = j;
        CHECK(result.fa[i] == closest);
        auto found = std::find(keys.begin(), keys.end(), expected[i]);
        CHECK(result.inputIndex[i] == (found == keys.end() ? -1 : int(found - keys.begin())));
        if (closest != -1)
            CHECK(std::count(adjacency[closest].begin(), adjacency[closest].end(), i) == 1);
    }
}
struct Trace {
    inline static std::vector<std::tuple<int, int, int, int>> events;
};
template <int Id>
struct Hook : _tree::TreeAttribute {
    template <class Core>
    void enter(Core &, int x, int p) {
        Trace::events.emplace_back(Id, x, -1, p);
    }
    template <class Core>
    void leave(Core &, int x, int p) {
        Trace::events.emplace_back(Id + 2, x, -1, p);
    }
    template <class Core, class Edge>
    void afterChild(Core &, int x, int y, int p, Edge &) {
        Trace::events.emplace_back(Id + 4, x, y, p);
    }
};
#include "../../../Support/CaseSupport.hpp"
void verifyAdded(std::vector<std::vector<int>> g) {
    if (g.empty()) {
        FullTree<int> t(g);
        CHECK(t.n == 0 and t.root == -1);
        return;
    }
    for (int root = 0; root < int(g.size()); ++root) {
        auto adj = g;
        int n = int(adj.size());
        std::vector<int> fa(n, -1), dep(n), q{root};
        for (std::size_t i = 0; i < q.size(); ++i)
            for (int y : adj[q[i]])
                if (y != fa[q[i]]) {
                    fa[y] = q[i];
                    dep[y] = dep[q[i]] + 1;
                    q.push_back(y);
                }
        FullTree<int> t(g, root);
        CHECK(t.fa == fa and t.dep == dep);
        for (int x = 0; x < n; ++x)
            for (int y = 0; y < n; ++y) {
                int a = ancestor(x, y, fa, dep);
                CHECK(t.getLca(x, y) == a);
                CHECK(t.dist(x, y) == dep[x] + dep[y] - 2 * dep[a]);
            }
        checkVirtual(t, {0, n - 1, 0}, {root}, fa, dep);
    }
}

int main() {
    runCase("Tree/01-empty", [] {
        verifyAdded({});
    });
    runCase("Tree/02-single", [] {
        verifyAdded({{}});
    });
    runCase("Tree/03-edge", [] {
        verifyAdded({{1}, {0}});
    });
    runCase("Tree/04-chain", [] {
        verifyAdded({{1}, {0, 2}, {1, 3}, {2, 4}, {3}});
    });
    runCase("Tree/05-star", [] {
        verifyAdded({{1, 2, 3, 4}, {0}, {0}, {0}, {0}});
    });
    runCase("Tree/06-balanced", [] {
        verifyAdded({{1, 2}, {0, 3, 4}, {0, 5, 6}, {1}, {1}, {2}, {2}});
    });
    runCase("Tree/07-unequal-branches", [] {
        verifyAdded({{1, 3}, {0, 2}, {1}, {0, 4}, {3, 5}, {4}});
    });
    runCase("Tree/08-reversed-neighbors", [] {
        verifyAdded({{2, 1}, {4, 3, 0}, {6, 5, 0}, {1}, {1}, {2}, {2}});
    });
    runCase("Tree/09-broom", [] {
        verifyAdded({{1}, {0, 2}, {1, 3, 4, 5}, {2}, {2}, {2}});
    });
    runCase("Tree/10-two-branches", [] {
        verifyAdded({{1, 4}, {0, 2}, {1, 3}, {2}, {0, 5}, {4}});
    });
    return finishCases(10);
}
