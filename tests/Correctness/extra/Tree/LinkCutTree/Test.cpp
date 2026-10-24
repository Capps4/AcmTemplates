#include "../../../../../src/extra/Tree/LinkCutTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

struct Word {
    std::string text;
    explicit Word(std::string text) : text(std::move(text)) {}
    Word operator+(const Word& b) const { return Word(text + b.text); }
};

std::vector<int> path(const std::vector<std::vector<int>>& adj, int x, int y) {
    int n = int(adj.size());
    std::vector<int> prev(n, -1), queue{x};
    prev[x] = x;
    for (std::size_t at = 0; at < queue.size(); ++at) {
        int u = queue[at];
        for (int v = 0; v < n; ++v) {
            if (adj[u][v] and prev[v] == -1) {
                prev[v] = u;
                queue.push_back(v);
            }
        }
    }
    if (prev[y] == -1) {
        return {};
    }
    std::vector<int> result{y};
    while (y != x) {
        y = prev[y];
        result.push_back(y);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

void verify(LinkCutTree<Word>& t, const std::vector<std::vector<int>>& adj,
            const std::vector<Word>& values, int x, int y) {
    auto nodes = path(adj, x, y);
    CHECK(t.connected(x, y) == not nodes.empty());
    auto result = t.query(x, y);
    CHECK(bool(result) == not nodes.empty());
    if (result) {
        std::string expected;
        for (int v : nodes) {
            expected += values[v].text;
        }
        CHECK(result->text == expected);
    }
}

int main() {
    runCase("LinkCutTree/corners", [] {
        LinkCutTree<Word> empty({});
        LinkCutTree<Word> t({Word("a"), Word("b"), Word("c")});
        CHECK(t.connected(0, 0));
        CHECK(not t.connected(0, 1));
        CHECK(not t.query(0, 1));
        CHECK(not t.link(0, 0));
        CHECK(not t.cut(0, 0));
        CHECK(t.link(0, 1));
        CHECK(t.link(1, 2));
        CHECK(not t.link(2, 0));
        CHECK(not t.cut(0, 2));
        CHECK(t.query(0, 2)->text == "abc");
        CHECK(t.query(2, 0)->text == "cba");
        t.modify(1, Word("xy"));
        CHECK(t.query(0, 2)->text == "axyc");
        CHECK(t.query(2, 0)->text == "cxya");
        CHECK(t.cut(2, 1));
        CHECK(not t.cut(2, 1));
        CHECK(not t.connected(0, 2));
        CHECK(t.link(2, 0));
        CHECK(t.query(2, 1)->text == "caxy");
        CHECK(t.query(1, 1)->text == "xy");
    });
    runCase("LinkCutTree/forest-exhaustive", [] {
        constexpr int n = 4;
        const std::vector<std::pair<int, int>> edges{{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}};
        const std::vector<Word> values{Word("a"), Word("b"), Word("c"), Word("d")};
        for (int mask = 0; mask < 64; ++mask) {
            std::vector<std::vector<int>> adj(n, std::vector<int>(n));
            LinkCutTree<Word> t(values);
            for (int k = 0; k < int(edges.size()); ++k) {
                if ((mask >> k) & 1) {
                    auto [x, y] = edges[k];
                    bool valid = path(adj, x, y).empty();
                    CHECK(t.link(x, y) == valid);
                    if (valid) {
                        adj[x][y] = adj[y][x] = 1;
                    }
                }
            }
            for (int x = 0; x < n; ++x) {
                for (int y = 0; y < n; ++y) {
                    verify(t, adj, values, x, y);
                }
            }
            for (auto [x, y] : edges) {
                bool present = adj[x][y];
                CHECK(t.cut(y, x) == present);
                adj[x][y] = adj[y][x] = 0;
                for (int a = 0; a < n; ++a) {
                    for (int b = 0; b < n; ++b) {
                        verify(t, adj, values, a, b);
                    }
                }
            }
        }
    });
    runCase("LinkCutTree/noncommutative-oracle", [] {
        constexpr int n = 18;
        std::vector<Word> values;
        for (int i = 0; i < n; ++i) {
            values.emplace_back(std::string(1, char('a' + i)));
        }
        LinkCutTree<Word> t(values);
        std::vector<std::vector<int>> adj(n, std::vector<int>(n));
        for (int step = 0; step < 1600; ++step) {
            test_context::step = step;
            int x = randomInt(0, n - 1), y = randomInt(0, n - 1);
            int op = randomInt(0, 3);
            if (op == 0) {
                bool valid = path(adj, x, y).empty();
                CHECK(t.link(x, y) == valid);
                if (valid) {
                    adj[x][y] = adj[y][x] = 1;
                }
            } else if (op == 1) {
                bool present = adj[x][y];
                CHECK(t.cut(x, y) == present);
                adj[x][y] = adj[y][x] = 0;
            } else if (op == 2) {
                values[x] = Word(std::string(1, char('a' + randomInt(0, 25))));
                t.modify(x, values[x]);
            }
            verify(t, adj, values, x, y);
            verify(t, adj, values, y, x);
            if (step % 80 == 0) {
                for (int a = 0; a < n; ++a) {
                    for (int b = 0; b < n; ++b) {
                        verify(t, adj, values, a, b);
                    }
                }
            }
        }
    });
    runCase("LinkCutTree/long-chain", [] {
        constexpr int n = 20000;
        LinkCutTree<Info> t(std::vector<Info>(n, Info(1)));
        for (int i = 1; i < n; ++i) {
            CHECK(t.link(i - 1, i));
        }
        for (int i = 0; i < n; i += 131) {
            CHECK(t.query(i, n - 1)->val == n - i);
            CHECK(t.query(n - 1, i)->val == n - i);
        }
        t.modify(n / 2, Info(7));
        CHECK(t.query(0, n - 1)->val == n + 6);
        CHECK(t.cut(n / 2 - 1, n / 2));
        CHECK(not t.query(0, n - 1));
        CHECK(t.link(n / 2 - 1, n - 1));
        CHECK(t.query(0, n / 2)->val == n + 6);
    });
    return 0;
}
