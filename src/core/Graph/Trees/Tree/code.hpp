#pragma once
#include "../../../../../Headers/Headers.hpp"
namespace _tree {

constexpr int endOf(int x) { return x; }
constexpr int wtOf(int) { return 1; }

template <class Weight>
constexpr int endOf(const std::pair<int, Weight>& e) { return e.first; }

template <class Weight>
constexpr const Weight& wtOf(const std::pair<int, Weight>& e) { return e.second; }

// Opaque edge payloads keep topology abilities without requiring arithmetic.
template <class T, class = void>
struct WeightType {
    using type = void;
};

template <class T>
struct WeightType<T, std::void_t<std::common_type_t<long long,
                      std::decay_t<decltype(wtOf(std::declval<const T&>()))>>>> {
    using type = std::common_type_t<long long,
                 std::decay_t<decltype(wtOf(std::declval<const T&>()))>>;
};

struct TreeAttribute {
    void init(int) {}

    template <class Core, class Edge>
    void enter(Core&, int, const Edge&) {}

    template <class Core, class Edge>
    void leave(Core&, int, const Edge&) {}
};

struct AttributeFa : TreeAttribute {
    std::vector<int> fa{};

    void init(int n) { fa.assign(n, -1); }

    template <class Core, class Edge>
    void enter(const Core&, int pt, const Edge& e) { fa[endOf(e)] = pt; }
};

struct AttributeDep : TreeAttribute {
    std::vector<int> dep{};

    void init(int n) { dep.assign(n, 0); }

    template <class Core, class Edge>
    void enter(const Core&, int pt, const Edge& e) {
        if (pt != -1) {
            dep[endOf(e)] = dep[pt] + 1;
        }
    }
};

template <class W = long long>
struct AttributeWtDep : TreeAttribute {
    std::vector<W> wtDep{};

    void init(int n) { wtDep.assign(n, W{}); }

    template <class Core, class Edge>
    void enter(const Core&, int pt, const Edge& e) {
        if (pt != -1) {
            wtDep[endOf(e)] = wtDep[pt] + static_cast<W>(wtOf(e));
        }
    }
};

template <>
struct AttributeWtDep<void> : TreeAttribute {};

struct AttributeSize : TreeAttribute {
    std::vector<int> size{};

    void init(int n) { size.assign(n, 1); }

    template <class Core, class Edge>
    void leave(const Core&, int pt, const Edge& e) {
        if (pt != -1) {
            size[pt] += size[endOf(e)];
        }
    }
};

struct AttributeHeavy : TreeAttribute {
    template <class Core, class Edge>
    void leave(Core& core, int pt, const Edge& e) {
        static_assert(Core::template stageIndex<AttributeSize> <=
                      Core::template stageIndex<AttributeHeavy>,
                      "Size must be computed in the same or an earlier stage than Heavy");
        auto& adj = core.adj[endOf(e)];
        std::size_t pos = adj.size();
        for (std::size_t i = 0; i < adj.size(); ++i) {
            int y = endOf(adj[i]);
            if (y != pt and (pos == adj.size() or core.size[y] > core.size[endOf(adj[pos])])) {
                pos = i;
            }
        }
        // Reorder only this completed node's edges; the incoming edge stays intact.
        if (pos != adj.size() and pos != 0) {
            std::swap(adj.front(), adj[pos]);
        }
    }
};

struct AttributeTop : TreeAttribute {
    std::vector<int> tp{};

    void init(int n) { tp.assign(n, 0); }

    template <class Core, class Edge>
    void enter(const Core& core, int pt, const Edge& e) {
        static_assert(Core::template stageIndex<AttributeHeavy> <
                      Core::template stageIndex<AttributeTop>,
                      "Heavy must be computed in an earlier stage than Top");
        int x = endOf(e);
        tp[x] = pt != -1 and endOf(core.adj[pt].front()) == x ? tp[pt] : x;
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

    template <class Core, class Edge>
    void enter(const Core&, int, const Edge& e) { dfn[endOf(e)] = cur++; }

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

    template <class Core, class Edge>
    void enter(const Core&, int, const Edge& e) { idfn[cur++] = endOf(e); }

private:
    int cur{};
};

// Reuse the chosen DFN instead of maintaining another counter.
template <class DfnAttribute>
struct AttributeIdfnFrom : TreeAttribute {
    std::vector<int> idfn{};

    void init(int n) { idfn.assign(n, 0); }

    template <class Core, class Edge>
    void enter(const Core& core, int, const Edge& e) {
        using ThisStage = typename Core::template StageOf<AttributeIdfnFrom>;
        static_assert(ThisStage::template attributeIndex<DfnAttribute> <
                      ThisStage::template attributeIndex<AttributeIdfnFrom>,
                      "Dfn must precede IdfnFrom within the same stage");
        int x = endOf(e);
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
    static constexpr bool contains = (std::is_same_v<Attribute, Attributes> or ...);

    template <class Attribute>
    static constexpr int attributeIndex = firstMatch<std::is_same_v<Attribute, Attributes>...>();

    void init([[maybe_unused]] int n) { (static_cast<Attributes&>(*this).init(n), ...); }

    template <class Core, class Edge>
    static void dfs(Core& core, int pt, const Edge& e) {
        int x = endOf(e);
        (static_cast<Attributes&>(core).enter(core, pt, e), ...);
        for (const auto& ne : core.adj[x]) {
            if (endOf(ne) != pt) {
                dfs(core, x, ne);
            }
        }
        (static_cast<Attributes&>(core).leave(core, pt, e), ...);
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
        : Stages()..., adj(graph), n(static_cast<int>(graph.size())), root(n ? root : -1) {
        (static_cast<Stages&>(*this).init(n), ...);
        if (n == 0)
            return;
        assert(root >= 0 and root < n);
        (Stages::dfs(*this, -1, root), ...);
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

template <class Derived, class W = long long>
struct AbilityWtDist {
    W wtDist(int x, int y) const {
        const auto& self = static_cast<const Derived&>(*this);
        int z = self.lca(x, y);
        return (self.wtDep[x] - self.wtDep[z]) + (self.wtDep[y] - self.wtDep[z]);
    }
};

template <class Derived>
struct AbilityWtDist<Derived, void> {};

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
        return dfn[x] <= dfn[y] and dfn[y] < dfn[x] + self.size[x];
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

template <class T, class W = typename WeightType<T>::type>
struct FullTree
    : TreeCore<T,
               Stage<AttributeFa,
                     AttributeDep,
                     AttributeWtDep<W>,
                     AttributeSize,
                     AttributeHeavy>,
               Stage<AttributeTop,
                     AttributeDfn<>,
                     AttributeIdfnFrom<AttributeDfn<>>>>,
      AbilityLca<FullTree<T, W>>,
      AbilityDist<FullTree<T, W>>,
      AbilityWtDist<FullTree<T, W>, W>,
      AbilityGetRoad<FullTree<T, W>>,
      AbilityKthAncestor<FullTree<T, W>>,
      AbilityIsAncestor<FullTree<T, W>>,
      AbilityVirtualTree<FullTree<T, W>> {
    using FullTree::TreeCore::TreeCore;
};

template <class T, class W = typename WeightType<T>::type>
struct LcaTree
    : TreeCore<T,
               Stage<AttributeFa,
                     AttributeDep,
                     AttributeWtDep<W>,
                     AttributeSize,
                     AttributeHeavy>,
               Stage<AttributeTop>>,
      AbilityLca<LcaTree<T, W>>,
      AbilityDist<LcaTree<T, W>>,
      AbilityWtDist<LcaTree<T, W>, W> {
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

template <class T, class W = typename _tree::WeightType<T>::type>
using FullTree = _tree::FullTree<T, W>;

template <class T, class W = typename _tree::WeightType<T>::type>
using LcaTree = _tree::LcaTree<T, W>;

template <class T>
using SimpleTree = _tree::SimpleTree<T>;

