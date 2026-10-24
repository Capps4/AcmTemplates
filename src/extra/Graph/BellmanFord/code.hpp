#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T>
class BellmanFord {
    static_assert(std::is_integral_v<T> and std::is_signed_v<T> and sizeof(T) <= 8);
    struct Edge {
        int x, y;
        T w;
    };
    int n;
    std::vector<Edge> ed{};

public:
    enum State { Unreachable, Finite, NegativeInfinity };
    struct Result {
        std::vector<State> state{};
        std::vector<__int128_t> dis{};
        std::vector<int> pre{}, cycle{};
    };

    explicit BellmanFord(int n) : n(n) {
        assert(n >= 0);
    }

    int add(int x, int y, T w) {
        assert(0 <= x and x < n and 0 <= y and y < n);
        assert(ed.size() < std::size_t(std::numeric_limits<int>::max()));
        int id = int(ed.size());
        ed.push_back({x, y, w});
        return id;
    }

    Result work(int s) const {
        assert(0 <= s and s < n);
        Result res;
        res.state.assign(n, Unreachable);
        res.dis.assign(n, 0);
        res.pre.assign(n, -1);
        res.state[s] = Finite;
        std::vector<int> q;
        int last = -1;
        for (int k = 0; k < n; ++k) {
            last = -1;
            for (int id = 0; id < int(ed.size()); ++id) {
                auto [x, y, w] = ed[id];
                if (res.state[x] == Unreachable) {
                    continue;
                }
                __int128_t d = res.dis[x] + w;
                if (res.state[y] == Unreachable or d < res.dis[y]) {
                    res.dis[y] = d;
                    res.pre[y] = id;
                    res.state[y] = Finite;
                    last = y;
                    if (k == n - 1) {
                        q.push_back(y);
                    }
                }
            }
            if (last == -1) {
                break;
            }
        }
        if (q.empty()) {
            return res;
        }
        for (int k = 0; k < n; ++k) {
            last = ed[res.pre[last]].x;
        }
        int x = last;
        do {
            res.cycle.push_back(res.pre[x]);
            x = ed[res.pre[x]].x;
        } while (x != last);
        std::reverse(res.cycle.begin(), res.cycle.end());

        std::vector<std::vector<int>> g(n);
        for (auto e : ed) {
            g[e.x].push_back(e.y);
        }
        int cnt = 0;
        for (int y : q) {
            if (res.state[y] != NegativeInfinity) {
                res.state[y] = NegativeInfinity;
                q[cnt++] = y;
            }
        }
        q.resize(cnt);
        for (std::size_t i = 0; i < q.size(); ++i) {
            for (int y : g[q[i]]) {
                if (res.state[y] != NegativeInfinity) {
                    res.state[y] = NegativeInfinity;
                    q.push_back(y);
                }
            }
        }
        return res;
    }
};
