#pragma once

namespace custom_edge {
struct Edge {
    int to;
    long long wt;
    Edge() = delete;
    Edge(int x, long long w) : to(x), wt(w) {}
};
int endOf(const Edge& e) { return e.to; }
const long long& wtOf(const Edge& e) { return e.wt; }
}

struct AttributeXor : _tree::TreeAttribute {
    std::vector<long long> xr{};
    void init(int n) { xr.assign(n, 0); }
    template <class Core, class Edge>
    void enter(const Core&, int pt, const Edge& e) {
        using _tree::endOf;
        using _tree::wtOf;
        if (pt != -1)
            xr[endOf(e)] = xr[pt] ^ wtOf(e);
    }
};

template <class Derived>
struct AbilityXor {
    long long pathXor(int x, int y) const {
        const auto& self = static_cast<const Derived&>(*this);
        return self.xr[x] ^ self.xr[y];
    }
};

// Placed after Heavy to catch accidental changes to the incoming edge in leave.
struct AttributeAudit : _tree::TreeAttribute {
    std::vector<long long> wt{};
    std::vector<int> cnt{};
    void init(int n) {
        wt.assign(n, 0);
        cnt.assign(n, 0);
    }
    template <class Core, class Edge>
    void enter(const Core&, int pt, const Edge& e) {
        using _tree::endOf;
        using _tree::wtOf;
        if (pt != -1)
            wt[endOf(e)] = wtOf(e);
    }
    template <class Core, class Edge>
    void leave(const Core&, int pt, const Edge& e) {
        using _tree::endOf;
        using _tree::wtOf;
        int x = endOf(e);
        CHECK(++cnt[x] == 1);
        if (pt != -1)
            CHECK(wt[x] == wtOf(e));
    }
};

template <class T>
struct XorTree
    : _tree::TreeCore<T,
          _tree::Stage<_tree::AttributeFa, _tree::AttributeDep,
                       _tree::AttributeSize, _tree::AttributeHeavy,
                       AttributeXor, AttributeAudit>,
          _tree::Stage<_tree::AttributeTop>>,
      _tree::AbilityLca<XorTree<T>>,
      AbilityXor<XorTree<T>> {
    using XorTree::TreeCore::TreeCore;
};

void weightedCases() {
    using Edge = std::pair<int, int>;
    static_assert(std::is_same_v<decltype(_tree::wtOf(std::declval<const Edge&>())), const int&>);
    static_assert(std::is_same_v<decltype(std::declval<FullTree<Edge>>().wtDist(0, 0)), long long>);
    static_assert(std::is_same_v<typename _tree::WeightType<std::pair<int, double>>::type, double>);
    static_assert(std::is_same_v<typename _tree::WeightType<std::pair<int, std::unique_ptr<int>>>::type, void>);
    CHECK(_tree::endOf(7) == 7 and _tree::wtOf(7) == 1);
    std::vector<std::vector<Edge>> empty;
    FullTree<Edge> et(empty);
    LcaTree<Edge> el(empty);
    CHECK(et.wtDep.empty() and el.wtDep.empty());
    std::vector<std::vector<Edge>> zero{{{1, 0}}, {{0, 0}, {2, 0}}, {{1, 0}}};
    FullTree<Edge> zt(zero, 1);
    LcaTree<Edge> zl(zero, 1);
    CHECK(zt.wtDep == std::vector<long long>(3, 0) and zl.wtDep == zt.wtDep);
    CHECK(zt.dist(0, 2) == 2 and zt.wtDist(0, 2) == 0 and zl.wtDist(0, 2) == 0);
    FullTree<Edge, void> off(zero, 1);
    CHECK(off.lca(0, 2) == 1 and off.dist(0, 2) == 2);
    std::vector<std::vector<int>> plain{{1}, {0, 2}, {1}};
    LcaTree<int> pl(plain, 2);
    CHECK(pl.wtDep == std::vector<long long>({2, 1, 0}));
    CHECK(pl.wtDist(0, 2) == pl.dist(0, 2));
    for (int trial = 0; trial < 60; ++trial) {
        test_context::step = trial;
        int n = randomInt(1, 40), root = randomInt(0, n - 1);
        std::vector<std::vector<Edge>> g(n);
        for (int x = 1; x < n; ++x) {
            int pt = randomInt(0, x - 1), w = randomInt(-1000000000, 1000000000);
            g[x].emplace_back(pt, w);
            g[pt].emplace_back(x, w);
        }
        for (auto& row : g)
            std::shuffle(row.begin(), row.end(), testRng);
        auto org = g;
        FullTree<Edge> ft(g, root);
        LcaTree<Edge> lt(g, root);
        XorTree<Edge> xt(g, root);
        std::vector<int> pt(n, -1), dep(n), q{root};
        std::vector<long long> dis(n);
        for (std::size_t i = 0; i < q.size(); ++i) {
            int x = q[i];
            for (auto [y, w] : org[x]) {
                if (y == pt[x])
                    continue;
                pt[y] = x;
                dep[y] = dep[x] + 1;
                dis[y] = dis[x] + w;
                q.push_back(y);
            }
        }
        CHECK(ft.dep == dep and lt.dep == dep);
        CHECK(ft.wtDep == dis and lt.wtDep == dis);
        CHECK(ft.fa == pt and lt.fa == pt);
        for (int x = 0; x < n; ++x) {
            std::vector<int> seen(n, -1), hops(n), todo{x};
            std::vector<long long> sum(n), xr(n);
            seen[x] = x;
            // Independent traversal from every source; no LCA/prefix formula.
            for (std::size_t i = 0; i < todo.size(); ++i) {
                int u = todo[i];
                for (auto [v, w] : org[u]) {
                    if (seen[v] != -1)
                        continue;
                    seen[v] = u;
                    hops[v] = hops[u] + 1;
                    sum[v] = sum[u] + w;
                    xr[v] = xr[u] ^ w;
                    todo.push_back(v);
                }
            }
            for (int y = 0; y < n; ++y) {
                CHECK(ft.dist(x, y) == hops[y] and lt.dist(x, y) == hops[y]);
                CHECK(ft.wtDist(x, y) == sum[y] and lt.wtDist(x, y) == sum[y]);
                CHECK(xt.pathXor(x, y) == xr[y]);
                CHECK(ft.lca(x, y) == ancestor(x, y, pt, dep));
                long long total = 0;
                for (auto [l, r] : ft.getRoad(x, y)) {
                    int step = l <= r ? 1 : -1;
                    for (int j = l;; j += step) {
                        int v = ft.idfn[j];
                        if (v != ft.lca(x, y))
                            total += xt.wt[v];
                        if (j == r)
                            break;
                    }
                }
                CHECK(total == sum[y]);
            }
            auto row = g[x];
            std::sort(row.begin(), row.end());
            std::sort(org[x].begin(), org[x].end());
            CHECK(row == org[x]);
            int best = -1;
            for (auto [y, w] : g[x]) {
                (void)w;
                if (y != pt[x] and (best == -1 or ft.size[y] > ft.size[best]))
                    best = y;
            }
            if (best != -1)
                CHECK(g[x].front().first == best);
        }
    }
    std::vector<std::vector<std::pair<int, double>>> fp{
        {{1, 0.5}, {2, -1.25}}, {{0, 0.5}}, {{0, -1.25}}};
    FullTree<std::pair<int, double>> ft(fp, 1);
    LcaTree<std::pair<int, double>> lt(fp, 1);
    CHECK(ft.wtDep == std::vector<double>({0.5, 0, -0.75}));
    CHECK(ft.wtDist(1, 2) == -0.75 and lt.wtDist(1, 2) == -0.75);
    CHECK(ft.dep == std::vector<int>({1, 0, 2}) and ft.dist(1, 2) == 2);
    FullTree<std::pair<int, double>, long double> wide(fp);
    CHECK(wide.wtDist(1, 2) == -0.75L);
    std::vector<std::vector<custom_edge::Edge>> cg(3);
    cg[0].emplace_back(1, 9);
    cg[1].emplace_back(0, 9);
    cg[1].emplace_back(2, 12);
    cg[2].emplace_back(1, 12);
    FullTree<custom_edge::Edge> ct(cg, 2);
    XorTree<custom_edge::Edge> cx(cg, 2);
    CHECK(ct.wtDist(0, 2) == 21 and ct.dist(0, 2) == 2);
    CHECK(cx.pathXor(0, 2) == (9 ^ 12));
    // Large shared prefix: multiplying the LCA prefix by two would overflow.
    const long long mx = std::numeric_limits<long long>::max();
    std::vector<std::vector<std::pair<int, long long>>> big{
        {{1, mx - 10}}, {{0, mx - 10}, {2, 3}, {3, 4}}, {{1, 3}}, {{1, 4}}};
    FullTree<std::pair<int, long long>> bt(big);
    CHECK(bt.wtDist(2, 3) == 7 and bt.dist(2, 3) == 2);
}

void deepChain() {
    const int n = 200000, w = 1000000000;
    std::vector<std::vector<std::pair<int, int>>> g(n);
    for (int x = 1; x < n; ++x) {
        g[x - 1].emplace_back(x, w);
        g[x].emplace_back(x - 1, w);
    }
    FullTree<std::pair<int, int>> ft(g);
    LcaTree<std::pair<int, int>> lt(g, n - 1);
    for (int x = 0; x < n; ++x) {
        CHECK(ft.dep[x] == x and ft.wtDep[x] == 1LL * x * w);
        CHECK(lt.dep[x] == n - 1 - x and lt.wtDep[x] == 1LL * (n - 1 - x) * w);
    }
    CHECK(ft.size[0] == n and ft.kthAncestor(n - 1, n - 1) == 0);
    CHECK(ft.kthAncestor(n - 1, n) == -1 and ft.lca(0, n - 1) == 0);
    CHECK(ft.dist(0, n - 1) == n - 1 and lt.dist(0, n - 1) == n - 1);
    CHECK(ft.wtDist(0, n - 1) == 1LL * (n - 1) * w);
    CHECK(lt.wtDist(0, n - 1) == ft.wtDist(0, n - 1));
}
