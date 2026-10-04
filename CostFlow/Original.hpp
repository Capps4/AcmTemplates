#pragma once
#include <bits/stdc++.h>
using i64 = long long;
enum CostFlowMode {
    MAX_FLOW_MIN_COST, // 优先流量最大，然后费用最小
    MIN_COST_MAX_FLOW, // 优先费用最小，然后流量最大
    MIN_COST_MIN_FLOW  // 优先费用最小，然后流量最小
};

template <class Cap, class Fee>
class CostFlow {
    struct Edge {
        int to;
        Cap cap;
        Fee fee;

        Edge(int to, Cap cap, Fee fee)
            : to{to}, cap{cap}, fee{fee} {}
    };

    const Fee Inf = std::numeric_limits<Fee>::max();

    bool dijkstra(int s, int t) {
        dist.assign(n, Inf);
        pre.assign(n, -1);

        typedef std::pair<Fee, int> Pair;
        std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair> > q;

        dist[s] = 0;
        q.push(Pair(0, s));

        while (!q.empty()) {
            Pair cur = q.top();
            q.pop();

            Fee nearDist = cur.first;
            int x = cur.second;

            if (dist[x] < nearDist)
                continue;

            for (int i : adj[x]) {
                int y = edge[i].to;
                Cap cap = edge[i].cap;
                Fee fee = edge[i].fee;

                if (cap > 0 and dist[y] > nearDist + pot[x] - pot[y] + fee) {
                    dist[y] = nearDist + pot[x] - pot[y] + fee;
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

    std::vector<Fee> pot{}, dist{};
    std::vector<int> pre{};

    CostFlow(int n)
        : n(n), adj(n) {}

    void add(int x, int y, Cap cap, Fee fee) {
        adj[x].push_back((int)edge.size());
        edge.push_back(Edge(y, cap, fee));

        adj[y].push_back((int)edge.size());
        edge.push_back(Edge(x, 0, -fee));
    }

    template <CostFlowMode mode = MAX_FLOW_MIN_COST>
    std::pair<Cap, Fee> work(int s, int t) {
        assert(s != t);
        Cap flow = 0;
        Fee cost = 0;

        pot.assign(n, 0);

        while (dijkstra(s, t)) {
            for (int i = 0; i < n; ++i) {
                if (dist[i] != Inf) pot[i] += dist[i];
            }

            if (mode == MIN_COST_MAX_FLOW) {
                if (pot[t] > 0) break;
            }
            if (mode == MIN_COST_MIN_FLOW) {
                if (pot[t] >= 0) break;
            }

            Cap aug = std::numeric_limits<Cap>::max();

            for (int x = t; x != s; x = edge[pre[x] ^ 1].to)
                aug = std::min(aug, edge[pre[x]].cap);

            for (int x = t; x != s; x = edge[pre[x] ^ 1].to) {
                edge[pre[x]].cap -= aug;
                edge[pre[x] ^ 1].cap += aug;
            }

            flow += aug;
            cost += Fee(aug) * pot[t];
        }

        return std::make_pair(flow, cost);
    }
};

