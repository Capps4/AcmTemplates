#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/TreeTheory/Tree/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <memory>
#include <set>

int ancestor(int x, int y, const std::vector<int>& parent, const std::vector<int>& depth) {
    while (depth[x] > depth[y]) x = parent[x];
    while (depth[y] > depth[x]) y = parent[y];
    while (x != y) { x = parent[x]; y = parent[y]; }
    return x;
}
bool isAncestor(int x, int y, const std::vector<int>& parent) {
    while (y != -1) { if (x == y) return true; y = parent[y]; }
    return false;
}
void checkVirtual(const FullTree<int>& tree, const std::vector<int>& keys, const std::vector<int>& extra,
                  const std::vector<int>& parent, const std::vector<int>& depth) {
    std::set<int> closure(keys.begin(), keys.end()); closure.insert(extra.begin(), extra.end());
    auto initial = closure;
    for (int x : initial) for (int y : initial) closure.insert(ancestor(x, y, parent, depth));
    std::vector<int> expected(closure.begin(), closure.end());
    std::sort(expected.begin(), expected.end(), [&](int x, int y) { return tree.dfn[x] < tree.dfn[y]; });
    auto result = tree.getVirtualTree(keys, extra);
    CHECK(result.dfsOrder == expected);
    auto adjacency = result.adj();
    for (int i = 0; i < int(expected.size()); ++i) {
        int closest = -1;
        for (int j = 0; j < i; ++j) if (isAncestor(expected[j], expected[i], parent) &&
            (closest == -1 || depth[expected[j]] > depth[expected[closest]])) closest = j;
        CHECK(result.fa[i] == closest);
        auto found = std::find(keys.begin(), keys.end(), expected[i]);
        CHECK(result.inputIndex[i] == (found == keys.end() ? -1 : int(found - keys.begin())));
        if (closest != -1) CHECK(std::count(adjacency[closest].begin(), adjacency[closest].end(), i) == 1);
    }
}
struct Trace { inline static std::vector<std::tuple<int, int, int, int>> events; };
template<int Id> struct Hook : _tree::TreeAttribute {
    template<class Core> void enter(Core&, int x, int p) { Trace::events.emplace_back(Id, x, -1, p); }
    template<class Core> void leave(Core&, int x, int p) { Trace::events.emplace_back(Id + 2, x, -1, p); }
    template<class Core, class Edge> void afterChild(Core&, int x, int y, int p, Edge&) { Trace::events.emplace_back(Id + 4, x, y, p); }
};
int coreCases() {
    std::vector<std::vector<int>> empty;
    FullTree<int> emptyTree(empty);
    CHECK(emptyTree.n == 0 && emptyTree.root == -1 && emptyTree.getVirtualTree({}).adj().empty());
    std::vector<std::vector<int>> fixture{{1, 2}, {0}, {0}};
    _tree::TreeCore<int, _tree::Stage<Hook<0>, Hook<1>>> traced(fixture);
    CHECK(Trace::events == std::vector<std::tuple<int,int,int,int>>({
        {0,0,-1,-1},{1,0,-1,-1},{0,1,-1,0},{1,1,-1,0},{2,1,-1,0},{3,1,-1,0},{4,0,1,-1},{5,0,1,-1},
        {0,2,-1,0},{1,2,-1,0},{2,2,-1,0},{3,2,-1,0},{4,0,2,-1},{5,0,2,-1},{2,0,-1,-1},{3,0,-1,-1}}));
    for (int trial = 0; trial < 8; ++trial) {
        test_context::step = trial;
        int n = randomInt(1, 70), root = randomInt(0, n - 1);
        std::vector<std::vector<int>> graph(n);
        for (int x = 1; x < n; ++x) { int y = randomInt(0, x - 1); graph[x].push_back(y); graph[y].push_back(x); }
        for (auto& row : graph) std::shuffle(row.begin(), row.end(), testRng);
        auto original = graph;
        std::vector<int> parent(n, -1), depth(n), queue{root}, size(n);
        for (std::size_t head = 0; head < queue.size(); ++head) for (int y : original[queue[head]]) if (y != parent[queue[head]]) {
            parent[y] = queue[head]; depth[y] = depth[queue[head]] + 1; queue.push_back(y);
        }
        for (int x = 0; x < n; ++x) for (int y = x; y != -1; y = parent[y]) ++size[y];
        FullTree<int> tree(graph, root);
        CHECK(tree.fa == parent && tree.dep == depth && tree.size == size);
        for (int x = 0; x < n; ++x) {
            CHECK(tree.idfn[tree.dfn[x]] == x);
            for (int y = 0; y < n; ++y) CHECK(tree.isAncestor(x, y) == isAncestor(x, y, parent));
            for (int k = 0; k <= n + 1; ++k) {
                int expected = x; for (int step = 0; step < k && expected != -1; ++step) expected = parent[expected];
                CHECK(tree.kthAncestor(x, k) == expected);
            }
        }
        for (int query = 0; query < 8; ++query) {
        test_context::step = query;
            int x = randomInt(0, n - 1), y = randomInt(0, n - 1), common = ancestor(x, y, parent, depth);
            CHECK(tree.getLca(x, y) == common && tree.dist(x, y) == depth[x] + depth[y] - 2 * depth[common]);
            std::vector<int> path, down, actual;
            for (int v = x; v != common; v = parent[v]) path.push_back(v);
            path.push_back(common);
            for (int v = y; v != common; v = parent[v]) down.push_back(v);
            path.insert(path.end(), down.rbegin(), down.rend());
            for (auto [l, r] : tree.getRoad(x, y)) {
                CHECK(0 <= l and l < n and 0 <= r and r < n);
                int step = l <= r ? 1 : -1;
                for (int i = l;; i += step) { actual.push_back(tree.idfn[i]); if (i == r) break; }
            }
            CHECK(actual == path);
        }
        for (int query = 0; query < 4; ++query) {
        test_context::step = query;
            std::vector<int> keys(randomInt(0, 12)), extra(randomInt(0, 5));
            for (auto& x : keys) x = randomInt(0, n - 1);
            for (auto& x : extra) x = randomInt(0, n - 1);
            checkVirtual(tree, keys, extra, parent, depth);
        }
    }
    std::vector<std::vector<std::pair<int, std::unique_ptr<int>>>> weighted(3);
    for (int x = 1; x < 3; ++x) { weighted[0].emplace_back(x, std::make_unique<int>(x)); weighted[x].emplace_back(0, std::make_unique<int>(x)); }
    FullTree<std::pair<int, std::unique_ptr<int>>> moveOnly(weighted);
    CHECK(moveOnly.dist(1, 2) == 2 && *weighted[0][0].second == weighted[0][0].first);
    static_assert(!std::is_constructible_v<FullTree<int>, std::vector<std::vector<int>>&&>);
    const int n = 128;
    std::vector<std::vector<int>> chain(n);
    for (int x = 1; x < n; ++x) { chain[x - 1].push_back(x); chain[x].push_back(x - 1); }
    FullTree<int> deep(chain);
    CHECK(deep.size[0] == n && deep.kthAncestor(n - 1, n - 1) == 0 && deep.kthAncestor(n - 1, n) == -1);
    CHECK(deep.lca(0, n - 1) == 0 && deep.dist(0, n - 1) == n - 1);
    std::cout << "Tree independent parent/path/LCA/virtual closure oracle, stage hook order, move-only weights, empty, 200K chain PASS\n";
    return 0;
}

#include "../../../../src/TreeTheory/Tree/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <memory>
#include <set>
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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

int run() {
    runCase("Tree/empty", [] {
        verifyAdded({});
    });
    runCase("Tree/single", [] {
        verifyAdded({{}});
    });
    runCase("Tree/edge", [] {
        verifyAdded({{1}, {0}});
    });
    runCase("Tree/chain", [] {
        verifyAdded({{1}, {0, 2}, {1, 3}, {2, 4}, {3}});
    });
    runCase("Tree/star", [] {
        verifyAdded({{1, 2, 3, 4}, {0}, {0}, {0}, {0}});
    });
    runCase("Tree/balanced", [] {
        verifyAdded({{1, 2}, {0, 3, 4}, {0, 5, 6}, {1}, {1}, {2}, {2}});
    });
    runCase("Tree/unequal-branches", [] {
        verifyAdded({{1, 3}, {0, 2}, {1}, {0, 4}, {3, 5}, {4}});
    });
    runCase("Tree/reversed-neighbors", [] {
        verifyAdded({{2, 1}, {4, 3, 0}, {6, 5, 0}, {1}, {1}, {2}, {2}});
    });
    runCase("Tree/broom", [] {
        verifyAdded({{1}, {0, 2}, {1, 3, 4, 5}, {2}, {2}, {2}});
    });
    runCase("Tree/two-branches", [] {
        verifyAdded({{1, 4}, {0, 2}, {1, 3}, {2}, {0, 5}, {4}});
    });
    return 0;
}
}

int main() {
    runCase("Tree/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
