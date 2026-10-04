#pragma once
#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
enum CostFlowMode { MAX_FLOW_MIN_COST, MIN_COST_MAX_FLOW, MIN_COST_MIN_FLOW };

template <class Cap, class Fee>
class CostFlow {
    static_assert(std::is_integral_v<Cap> && !std::is_same_v<Cap, bool> && sizeof(Cap) <= 8,
                  "CostFlow needs integral capacities up to 64 bits");
    static_assert(std::is_integral_v<Fee> && std::is_signed_v<Fee> && sizeof(Fee) <= 8,
                  "CostFlow needs signed integral fees up to 64 bits");
    using Wide = __int128_t;
    static constexpr Wide Inf = Wide(1) << 126;

    struct Edge {
        int to;
        Cap cap;
        Fee fee;
    };

    // Rebuild on every call: residual reverse arcs and newly added edges may be neg.
    void initPotential(int s) {
        pot.assign(n, 0);
        bool neg = false;
        for (const auto &arc : edge)
            neg |= arc.cap > 0 && arc.fee < 0;
        if (!neg) return;
        dist.assign(n, Inf);
        dist[s] = 0;
        std::deque<int> q{s};
        std::vector<bool> inq(n, false);
        std::vector<int> len(n, 0);
        inq[s] = true;
        while (!q.empty()) {
            int x = q.front();
            q.pop_front();
            inq[x] = false;
            for (int i : adj[x]) {
                const auto &arc = edge[i];
                int y = arc.to;
                if (arc.cap == 0 || dist[y] <= dist[x] + arc.fee) continue;
                dist[y] = dist[x] + arc.fee;
                len[y] = len[x] + 1;
                if (len[y] >= n) throw std::domain_error("CostFlow: reachable neg cycle");
                if (!inq[y]) {
                    q.push_back(y);
                    inq[y] = true;
                }
            }
        }
        for (int x = 0; x < n; ++x)
            if (dist[x] != Inf) pot[x] = dist[x];
    }

    bool dijkstra(int s, int t) {
        dist.assign(n, Inf);
        pre.assign(n, -1);
        using Entry = std::pair<Wide, int>;
        std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> q;
        dist[s] = 0;
        q.emplace(0, s);
        while (!q.empty()) {
            auto [dis, x] = q.top();
            q.pop();
            if (dis != dist[x]) continue;
            for (int i : adj[x]) {
                const auto &arc = edge[i];
                if (arc.cap == 0) continue;
                Wide w = Wide(arc.fee) + pot[x] - pot[arc.to];
                assert(w >= 0);
                Wide nd = dis + w;
                if (nd < dist[arc.to]) {
                    dist[arc.to] = nd;
                    pre[arc.to] = i;
                    q.emplace(nd, arc.to);
                }
            }
        }
        return dist[t] != Inf;
    }

public:
    const int n;
    std::vector<Edge> edge{};
    std::vector<std::vector<int>> adj;
    std::vector<Wide> pot{}, dist{};
    std::vector<int> pre{};

    explicit CostFlow(int n) : n(n), adj(n) {}

    void add(int x, int y, Cap cap, Fee fee) {
        assert(0 <= x && x < n && 0 <= y && y < n);
        assert(cap >= 0 && fee != std::numeric_limits<Fee>::min());
        int i = int(edge.size());
        adj[x].push_back(i);
        edge.push_back({y, cap, fee});
        adj[y].push_back(i + 1);
        edge.push_back({x, 0, Fee(-fee)});
    }

    // Additional flow/cost only. Capacities remain in the residual network.
    template <CostFlowMode mode = MAX_FLOW_MIN_COST>
    std::pair<Cap, Fee> work(int s, int t, Cap lim = std::numeric_limits<Cap>::max()) {
        static_assert(mode == MAX_FLOW_MIN_COST || mode == MIN_COST_MAX_FLOW ||
                      mode == MIN_COST_MIN_FLOW);
        assert(0 <= s && s < n && 0 <= t && t < n);
        assert(lim >= 0);
        if (s == t || lim == 0) return {0, 0};
        Cap flow = 0;
        Fee cost = 0;
        initPotential(s);
        while (flow < lim && dijkstra(s, t)) {
            for (int x = 0; x < n; ++x)
                if (dist[x] != Inf) pot[x] += dist[x];
            Wide fee = pot[t] - pot[s];
            if constexpr (mode == MIN_COST_MAX_FLOW) {
                if (fee > 0) break;
            }
            if constexpr (mode == MIN_COST_MIN_FLOW) {
                if (fee >= 0) break;
            }
            Cap aug = lim - flow;
            for (int x = t; x != s; x = edge[pre[x] ^ 1].to)
                aug = std::min(aug, edge[pre[x]].cap);
            Wide dc, cost2;
            if (__builtin_mul_overflow(Wide(aug), fee, &dc) ||
                __builtin_add_overflow(Wide(cost), dc, &cost2) ||
                cost2 < std::numeric_limits<Fee>::min() || cost2 > std::numeric_limits<Fee>::max())
                throw std::overflow_error("CostFlow: result cost does not fit Fee");
            for (int x = t; x != s; x = edge[pre[x] ^ 1].to) {
                edge[pre[x]].cap -= aug;
                edge[pre[x] ^ 1].cap += aug;
            }
            flow += aug;
            cost = Fee(cost2);
        }
        return {flow, cost};
    }
};
