#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
enum CostFlowMode {
    MAX_FLOW_MIN_COST, // 优先流量最大，然后费用最小
    MIN_COST_MAX_FLOW, // 优先费用最小，然后流量最大
    MIN_COST_MIN_FLOW  // 优先费用最小，然后流量最小
};

template <class Cap, class Fee>
class CostFlow {
    static_assert(std::is_integral_v<Cap> and sizeof(Cap) <= 8);
    static_assert(std::is_integral_v<Fee> and std::is_signed_v<Fee> and sizeof(Fee) <= 8);
    struct Edge {
        int to;
        Cap cap;
        Fee fee;

        Edge(int to, Cap cap, Fee fee)
            : to{to}, cap{cap}, fee{fee} {}
    };

    using Wide = __int128_t;
    static constexpr Wide Inf = Wide(1) << 126;

    // Rebuild on every call: residual reverse arcs and newly added edges may be neg.
    void init(int s) {
        pot.assign(n, 0);
        bool neg = false;
        for (const auto &arc : edge)
            neg |= arc.cap > 0 and arc.fee < 0;
        if (!neg)
            return;
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
                if (arc.cap == 0 or dist[y] <= dist[x] + arc.fee)
                    continue;
                dist[y] = dist[x] + arc.fee;
                len[y] = len[x] + 1;
                if (len[y] >= n)
                    throw std::domain_error("CostFlow: reachable neg cycle");
                if (!inq[y]) {
                    q.push_back(y);
                    inq[y] = true;
                }
            }
        }
        for (int x = 0; x < n; ++x)
            if (dist[x] != Inf)
                pot[x] = dist[x];
    }

    bool dijkstra(int s, int t) {
        dist.assign(n, Inf);
        pre.assign(n, -1);

        using Pair = std::pair<Wide, int>;
        std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> q;

        dist[s] = 0;
        q.push(Pair(0, s));

        while (!q.empty()) {
            auto [d, x] = q.top();
            q.pop();

            if (dist[x] < d)
                continue;

            for (int i : adj[x]) {
                int y = edge[i].to;
                Cap cap = edge[i].cap;
                Wide fee = edge[i].fee;

                if (cap > 0 and dist[y] > d + pot[x] - pot[y] + fee) {
                    dist[y] = d + pot[x] - pot[y] + fee;
                    pre[y] = i;
                    q.push(Pair(dist[y], y));
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

    CostFlow(int n)
        : n(n), adj(n) {}

    void add(int x, int y, Cap cap, Fee fee) {
        assert(cap >= 0 and fee != std::numeric_limits<Fee>::min());
        adj[x].push_back((int)edge.size());
        edge.push_back(Edge(y, cap, fee));

        adj[y].push_back((int)edge.size());
        edge.push_back(Edge(x, 0, -fee));
    }

    // Additional flow/cost only. Capacities remain in the residual network.
    template <CostFlowMode mode = MAX_FLOW_MIN_COST>
    std::pair<Cap, Fee> work(int s, int t, Cap lim = std::numeric_limits<Cap>::max()) {
        static_assert(mode == MAX_FLOW_MIN_COST or mode == MIN_COST_MAX_FLOW or
                      mode == MIN_COST_MIN_FLOW);
        assert(0 <= s and s < n and 0 <= t and t < n);
        assert(lim >= 0);
        if (s == t or lim == 0)
            return {0, 0};
        Cap flow = 0;
        Fee cost = 0;
        init(s);
        while (flow < lim and dijkstra(s, t)) {
            for (int x = 0; x < n; ++x)
                if (dist[x] != Inf)
                    pot[x] += dist[x];
            Wide fee = pot[t] - pot[s];
            if constexpr (mode == MIN_COST_MAX_FLOW) {
                if (fee > 0)
                    break;
            }
            if constexpr (mode == MIN_COST_MIN_FLOW) {
                if (fee >= 0)
                    break;
            }
            Cap aug = lim - flow;
            for (int x = t; x != s; x = edge[pre[x] ^ 1].to)
                aug = std::min(aug, edge[pre[x]].cap);
            Wide dc, sum;
            if (__builtin_mul_overflow(Wide(aug), fee, &dc) or
                __builtin_add_overflow(Wide(cost), dc, &sum) or
                sum < std::numeric_limits<Fee>::min() or sum > std::numeric_limits<Fee>::max())
                throw std::overflow_error("CostFlow: result cost does not fit Fee");
            for (int x = t; x != s; x = edge[pre[x] ^ 1].to) {
                edge[pre[x]].cap -= aug;
                edge[pre[x] ^ 1].cap += aug;
            }
            flow += aug;
            cost = Fee(sum);
        }
        return {flow, cost};
    }
};
