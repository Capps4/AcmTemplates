#include "../../../../../src/extra/Tree/Rerooting/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

struct NoDefault {
    int val;
    explicit NoDefault(int val) : val(val) {}
};

using Graph = std::vector<std::vector<int>>;

void checkGraph(const Graph &g) {
    int n = int(g.size());
    using Info = std::pair<long long, long long>;
    auto merge = [](Info a, Info b) { return Info{a.first + b.first, a.second + b.second}; };
    auto lift = [](Info a, int, int) { return Info{a.first, a.second + a.first}; };
    auto finish = [](Info a, int) { return Info{a.first + 1, a.second}; };
    Rerooting a(g, Info{}, merge, lift, finish);
    for (int s = 0; s < n; ++s) {
        std::vector<int> dis(n, -1), q{s};
        dis[s] = 0;
        long long sum = 0;
        for (int i = 0; i < int(q.size()); ++i)
            for (int y : g[q[i]])
                if (dis[y] == -1) {
                    dis[y] = dis[q[i]] + 1;
                    sum += dis[y];
                    q.push_back(y);
                }
        CHECK(a.ans[s] == Info(q.size(), sum));
    }
    auto cat = [](const std::string &a, const std::string &b) { return a + b; };
    auto edge = [](const std::string &a, int x, int y) {
        return "[" + std::to_string(x) + ">" + std::to_string(y) + a + "]";
    };
    auto node = [](const std::string &a, int x) { return "(" + std::to_string(x) + a + ")"; };
    Rerooting b(g, std::string{}, cat, edge, node);
    auto brute = [&](auto &&self, int x, int p) -> std::string {
        std::string s;
        for (int y : g[x])
            if (y != p)
                s += "[" + std::to_string(y) + ">" + std::to_string(x) + self(self, y, x) + "]";
        return "(" + std::to_string(x) + s + ")";
    };
    for (int x = 0; x < n; ++x)
        CHECK(b.ans[x] == brute(brute, x, -1));
}

int main() {
    runCase("Rerooting/forests-and-order", [] {
        checkGraph({});
        checkGraph({{}});
        checkGraph({{}, {}, {}});
        checkGraph({{2}, {2}, {0, 1}});
        checkGraph({{2, 1}, {3, 0}, {0}, {1}, {5}, {4}});
        checkGraph({{4, 3, 2, 1}, {0}, {0}, {0}, {0}});
    });
    runCase("Rerooting/increasing-forests", [] {
        for (int n = 1; n <= 6; ++n) {
            int total = 1;
            for (int x = 1; x < n; ++x)
                total *= x + 1;
            for (int mask = 0; mask < total; ++mask) {
                test_context::step = mask;
                int code = mask;
                Graph g(n);
                for (int x = 1; x < n; ++x) {
                    int p = code % (x + 1);
                    code /= x + 1;
                    if (p < x) {
                        g[x].push_back(p);
                        g[p].push_back(x);
                    }
                }
                checkGraph(g);
            }
        }
    });
    runCase("Rerooting/random-noncommutative", [] {
        for (int it = 0; it < 100; ++it) {
            test_context::step = it;
            int n = randomInt(1, 30);
            Graph g(n);
            for (int x = 1; x < n; ++x)
                if (randomInt(0, 3)) {
                    int p = randomInt(0, x - 1);
                    g[x].push_back(p);
                    g[p].push_back(x);
                }
            std::vector<int> labels(n);
            std::iota(labels.begin(), labels.end(), 0);
            std::shuffle(labels.begin(), labels.end(), testRng);
            Graph permuted(n);
            for (int x = 0; x < n; ++x)
                for (int y : g[x])
                    permuted[labels[x]].push_back(labels[y]);
            g = std::move(permuted);
            for (auto &a : g)
                std::shuffle(a.begin(), a.end(), testRng);
            checkGraph(g);
        }
    });
    runCase("Rerooting/non-default-value", [] {
        Rerooting a(Graph{{1}, {0, 2}, {1}, {}}, NoDefault(0), [](NoDefault x, NoDefault y) {
            return NoDefault(x.val + y.val);
        }, [](NoDefault x, int, int) {
            return x;
        }, [](NoDefault x, int) {
            return NoDefault(x.val + 1);
        });
        CHECK(a.ans[0].val == 3 and a.ans[1].val == 3 and a.ans[2].val == 3);
        CHECK(a.ans[3].val == 1);
    });
    runCase("Rerooting/deep-chain-and-star", [] {
        const int n = 20000;
        for (int shape = 0; shape < 2; ++shape) {
            Graph g(n);
            for (int x = 1; x < n; ++x) {
                int p = shape ? 0 : x - 1;
                g[x].push_back(p);
                g[p].push_back(x);
            }
            using Info = std::pair<long long, long long>;
            Rerooting a(g, Info{}, [](Info x, Info y) {
                return Info{x.first + y.first, x.second + y.second};
            }, [](Info x, int, int) {
                return Info{x.first, x.second + x.first};
            }, [](Info x, int) {
                return Info{x.first + 1, x.second};
            });
            for (long long x = 0; x < n; ++x) {
                long long want = shape ? (x == 0 ? n - 1 : 2LL * n - 3)
                    : x * (x + 1) / 2 + (n - x - 1) * (n - x) / 2;
                CHECK(a.ans[x] == Info(n, want));
            }
        }
    });
}
