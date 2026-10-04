#pragma once
#include <bits/stdc++.h>
using i64 = long long;
namespace _tree {

constexpr int endPoint(int x) { return x; }

template <class Weight>
constexpr int endPoint(const std::pair<int, Weight>& edge) { return edge.first; }

struct TreeAttribute {
    void init(int) {}

    template <class Core>
    void enter(Core&, int, int) {}

    template <class Core>
    void leave(Core&, int, int) {}

    template <class Core, class Edge>
    void afterChild(Core&, int, int, int, Edge&) {}
};

struct AttributeFa : TreeAttribute {
    std::vector<int> fa{};

    void init(int n) { fa.assign(n, -1); }

    template <class Core>
    void enter(const Core&, int x, int parent) { fa[x] = parent; }
};

struct AttributeDep : TreeAttribute {
    std::vector<int> dep{};

    void init(int n) { dep.assign(n, 0); }

    template <class Core>
    void enter(const Core&, int x, int parent) {
        if (parent != -1) {
            dep[x] = dep[parent] + 1;
        }
    }
};

struct AttributeSize : TreeAttribute {
    std::vector<int> size{};

    void init(int n) { size.assign(n, 1); }

    template <class Core>
    void leave(const Core&, int x, int parent) {
        if (parent != -1) {
            size[parent] += size[x];
        }
    }
};

struct AttributeHeavy : TreeAttribute {
    template <class Core, class Edge>
    void afterChild(Core& core, int x, int y, int parent, Edge& edge) {
        static_assert(Core::template stageIndex<AttributeSize> <=
                      Core::template stageIndex<AttributeHeavy>,
                      "Size must be computed in the same or an earlier stage than Heavy");
        auto& first = core.adj[x].front();
        const int z = endPoint(first);
        // Both slots have been visited; swapping cannot disturb unvisited children.
        if (z == parent || core.size[y] > core.size[z]) {
            std::swap(first, edge);
        }
    }
};

struct AttributeTop : TreeAttribute {
    std::vector<int> tp{};

    void init(int n) { tp.assign(n, 0); }

    template <class Core>
    void enter(const Core& core, int x, int parent) {
        static_assert(Core::template stageIndex<AttributeHeavy> <
                      Core::template stageIndex<AttributeTop>,
                      "Heavy must be computed in an earlier stage than Top");
        tp[x] = parent != -1 && endPoint(core.adj[parent].front()) == x ? tp[parent] : x;
    }
};

// A tag distinguishes multiple numbering arrays when needed.
template <class Tag = void>
struct AttributeDfn : TreeAttribute {
    std::vector<int> dfn{};

    void init(int n) {
        dfn.assign(n, 0);
        cur = 0;
    }

    template <class Core>
    void enter(const Core&, int x, int) { dfn[x] = cur++; }

private:
    int cur{};
};

template <class Tag = void>
struct AttributeIdfn : TreeAttribute {
    std::vector<int> idfn{};

    void init(int n) {
        idfn.assign(n, 0);
        cur = 0;
    }

    template <class Core>
    void enter(const Core&, int x, int) { idfn[cur++] = x; }

private:
    int cur{};
};

// Reuse the chosen DFN instead of maintaining another counter.
template <class DfnAttribute>
struct AttributeIdfnFrom : TreeAttribute {
    std::vector<int> idfn{};

    void init(int n) { idfn.assign(n, 0); }

    template <class Core>
    void enter(const Core& core, int x, int) {
        using ThisStage = typename Core::template StageOf<AttributeIdfnFrom>;
        static_assert(ThisStage::template attributeIndex<DfnAttribute> <
                      ThisStage::template attributeIndex<AttributeIdfnFrom>,
                      "Dfn must precede IdfnFrom within the same stage");
        idfn[static_cast<const DfnAttribute&>(core).dfn[x]] = x;
    }
};

// A missing match returns sizeof...(Matches); no dependency graph is built.
template <bool... Matches>
constexpr int firstMatch() {
    constexpr bool matches[] = {Matches..., true};
    int index{};
    while (!matches[index]) {
        ++index;
    }
    return index;
}

// This list determines both storage and hook order within one DFS.
template <class... Attributes>
struct Stage : Attributes... {
    template <class Attribute>
    static constexpr bool contains = (std::is_same_v<Attribute, Attributes> || ...);

    template <class Attribute>
    static constexpr int attributeIndex = firstMatch<std::is_same_v<Attribute, Attributes>...>();

    void init([[maybe_unused]] int n) { (static_cast<Attributes&>(*this).init(n), ...); }

    template <class Core>
    static void run(Core& core, int x, int parent) {
        (static_cast<Attributes&>(core).enter(core, x, parent), ...);
        visit(core, x, parent);
        (static_cast<Attributes&>(core).leave(core, x, parent), ...);
    }

    template <class Core>
    static void visit(Core& core, int x, int parent) {
        for (auto& edge : core.adj[x]) {
            const int y = endPoint(edge);
            if (y != parent) {
                run(core, y, x);
                (static_cast<Attributes&>(core).afterChild(core, x, y, parent, edge), ...);
            }
        }
    }
};

// Stages execute exactly as listed; attributes do not get reordered or added.
template <class T, class... Stages>
class TreeCore : public Stages... {
public:
    static constexpr int passCount = sizeof...(Stages);

    // Absent attributes map to passCount / void.
    template <class Attribute>
    static constexpr int stageIndex = firstMatch<Stages::template contains<Attribute>...>();

    template <class Attribute>
    using StageOf = std::tuple_element_t<stageIndex<Attribute>, std::tuple<Stages..., void>>;

    // Borrowed graph: Heavy reorders whole edges in place; keep the graph alive.
    std::vector<std::vector<T>>& adj;
    const int n;
    const int root;

    explicit TreeCore(std::vector<std::vector<T>>& graph, int root = 0)
        : Stages()..., adj(graph), n(static_cast<int>(graph.size())), root(root) {
        assert(root >= 0 && root < n);
        (static_cast<Stages&>(*this).init(n), ...);
        (Stages::run(*this, root, -1), ...);
    }
};

template <class Derived>
struct AbilityLca {
    int lca(int x, int y) const {
        const auto& self = static_cast<const Derived&>(*this);
        while (self.tp[x] != self.tp[y]) {
            if (self.dep[self.tp[x]] > self.dep[self.tp[y]]) {
                x = self.fa[self.tp[x]];
            } else {
                y = self.fa[self.tp[y]];
            }
        }
        return self.dep[x] < self.dep[y] ? x : y;
    }

    int getLca(int x, int y) const { return lca(x, y); }
};

template <class Derived>
struct AbilityDist {
    // Edge count, including when adjacency entries carry weights.
    int dist(int x, int y) const {
        const auto& self = static_cast<const Derived&>(*this);
        return self.dep[x] + self.dep[y] - 2 * self.dep[self.lca(x, y)];
    }
};

template <class Derived, class DfnAttribute = AttributeDfn<>>
struct AbilityGetRoad {
    // Closed DFN intervals in x -> y order; l > r means walking upward.
    std::vector<std::pair<int, int>> getRoad(int x, int y) const {
        static_assert(Derived::template stageIndex<AttributeHeavy> <
                      Derived::template stageIndex<DfnAttribute>,
                      "getRoad requires consecutive heavy-path numbering");

        const auto& self = static_cast<const Derived&>(*this);
        const auto& dfn = static_cast<const DfnAttribute&>(self).dfn;
        const int lca = self.lca(x, y);
        std::vector<std::pair<int, int>> up{}, down{};

        while (self.tp[x] != self.tp[lca]) {
            up.push_back({dfn[x], dfn[self.tp[x]]});
            x = self.fa[self.tp[x]];
        }
        if (x != lca) {
            up.push_back({dfn[x], dfn[lca] + 1});
        }
        up.push_back({dfn[lca], dfn[lca]});

        while (self.tp[y] != self.tp[lca]) {
            down.push_back({dfn[self.tp[y]], dfn[y]});
            y = self.fa[self.tp[y]];
        }
        if (y != lca) {
            down.push_back({dfn[lca] + 1, dfn[y]});
        }
        up.insert(up.end(), down.rbegin(), down.rend());
        return up;
    }
};

template <class Derived,
         class DfnAttribute = AttributeDfn<>,
         class IdfnAttribute = AttributeIdfnFrom<DfnAttribute>>
struct AbilityKthAncestor {
    int kthAncestor(int x, int k) const {
        static_assert(Derived::template stageIndex<DfnAttribute> ==
                      Derived::template stageIndex<IdfnAttribute>,
                      "Dfn and Idfn must belong to the same traversal");
        static_assert(Derived::template stageIndex<AttributeHeavy> <
                      Derived::template stageIndex<DfnAttribute>,
                      "kthAncestor requires consecutive heavy-path numbering");

        const auto& self = static_cast<const Derived&>(*this);
        if (k > self.dep[x] - self.dep[self.root]) {
            return -1;
        }
        const int depth = self.dep[x] - k;
        while (self.dep[self.tp[x]] > depth) {
            x = self.fa[self.tp[x]];
        }
        const auto& dfn = static_cast<const DfnAttribute&>(self).dfn;
        const auto& idfn = static_cast<const IdfnAttribute&>(self).idfn;
        return idfn[dfn[x] - self.dep[x] + depth];
    }
};

template <class Derived, class DfnAttribute = AttributeDfn<>>
struct AbilityIsAncestor {
    bool isAncestor(int x, int y) const {
        const auto& self = static_cast<const Derived&>(*this);
        const auto& dfn = static_cast<const DfnAttribute&>(self).dfn;
        return dfn[x] <= dfn[y] && dfn[y] < dfn[x] + self.size[x];
    }
};

struct VirtualTree {
    std::vector<int> dfsOrder{};       // Original vertex IDs in DFN order.
    std::vector<int> fa{};      // Local fa indices; root is 0 with fa -1.
    std::vector<int> inputIndex{};  // First position in keys; -1 for added vertices.

    std::vector<std::vector<int>> adj() const {
        std::vector<std::vector<int>> result(dfsOrder.size());
        for (int i = 1; i < static_cast<int>(dfsOrder.size()); ++i) {
            result[fa[i]].push_back(i);
        }
        return result;
    }
};

template <class Derived, class DfnAttribute = AttributeDfn<>>
struct AbilityVirtualTree {
    // Extra vertices are retained without becoming keys or changing the root direction.
    VirtualTree getVirtualTree(const std::vector<int>& keys,
                              const std::vector<int>& extra = {}) const {
        const auto& self = static_cast<const Derived&>(*this);
        const auto& dfn = static_cast<const DfnAttribute&>(self).dfn;
        const int keyCount = static_cast<int>(keys.size());
        std::vector<std::pair<int, int>> points{};
        points.reserve(2 * (keys.size() + extra.size()));
        for (int i = 0; i < keyCount; ++i) {
            points.emplace_back(keys[i], i);
        }
        for (int x : extra) {
            points.emplace_back(x, keyCount);
        }
        const auto sortUnique = [&] {
            std::sort(points.begin(), points.end(), [&](const auto& a, const auto& b) {
                return dfn[a.first] != dfn[b.first] ? dfn[a.first] < dfn[b.first]
                                                  : a.second < b.second;
            });
            points.erase(std::unique(points.begin(), points.end(), [](const auto& a, const auto& b) {
                return a.first == b.first;
            }), points.end());
        };
        sortUnique();
        const int count = static_cast<int>(points.size());
        for (int i = 1; i < count; ++i) {
            points.emplace_back(self.lca(points[i - 1].first, points[i].first), keyCount);
        }
        sortUnique();

        VirtualTree result{};
        const int m = static_cast<int>(points.size());
        result.dfsOrder.resize(m);
        result.fa.assign(m, -1);
        result.inputIndex.assign(m, -1);
        std::vector<int> stack{};
        stack.reserve(m);
        for (int i = 0; i < m; ++i) {
            result.dfsOrder[i] = points[i].first;
            if (points[i].second < keyCount) {
                result.inputIndex[i] = points[i].second;
            }
            if (i != 0) {
                const int ancestor = self.lca(points[i - 1].first, points[i].first);
                // LCA closure guarantees this ancestor is already on the stack.
                while (points[stack.back()].first != ancestor) {
                    stack.pop_back();
                }
                result.fa[i] = stack.back();
            }
            stack.push_back(i);
        }
        return result;
    }
};

template <class T>
struct FullTree
    : TreeCore<T,
               Stage<AttributeFa,
                     AttributeDep,
                     AttributeSize,
                     AttributeHeavy>,
               Stage<AttributeTop,
                     AttributeDfn<>,
                     AttributeIdfnFrom<AttributeDfn<>>>>,
      AbilityLca<FullTree<T>>,
      AbilityDist<FullTree<T>>,
      AbilityGetRoad<FullTree<T>>,
      AbilityKthAncestor<FullTree<T>>,
      AbilityIsAncestor<FullTree<T>>,
      AbilityVirtualTree<FullTree<T>> {
    using FullTree::TreeCore::TreeCore;
};

template <class T>
struct LcaTree
    : TreeCore<T,
               Stage<AttributeFa,
                     AttributeDep,
                     AttributeSize,
                     AttributeHeavy>,
               Stage<AttributeTop>>,
      AbilityLca<LcaTree<T>>,
      AbilityDist<LcaTree<T>> {
    using LcaTree::TreeCore::TreeCore;
};

template <class T>
struct SimpleTree
    : TreeCore<T,
               Stage<AttributeFa,
                     AttributeDep,
                     AttributeSize>> {
    using SimpleTree::TreeCore::TreeCore;
};

} // namespace _tree

template <class T>
using FullTree = _tree::FullTree<T>;

template <class T>
using LcaTree = _tree::LcaTree<T>;

template <class T>
using SimpleTree = _tree::SimpleTree<T>;

