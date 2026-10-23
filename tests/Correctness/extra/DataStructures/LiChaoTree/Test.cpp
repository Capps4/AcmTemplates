#include "../../../../../src/extra/DataStructures/LiChaoTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("LiChaoTree/oracle", [] {
        struct Line {
            using Key = long long;
            using Value = long long;
            long long k, b;
            long long operator[](long long x) const { return k * x + b; }
        };
        struct Item {
            int l, r;
            Line line;
        };
        const std::vector<long long> xs{-10, -3, 0, 2, 11};
        auto check = [&](const auto& tree, const std::vector<Item>& lines, bool minimum) {
            for (int x = 0; x < int(xs.size()); ++x) {
                bool covered = false;
                long long best = 0;
                for (const auto& q : lines) {
                    if (q.l <= x and x <= q.r) {
                        long long value = q.line.k * xs[x] + q.line.b;
                        if (!covered or (minimum ? value < best : value > best)) {
                            best = value;
                        }
                        covered = true;
                    }
                }
                CHECK(covered);
                CHECK(tree.query(xs[x]) == best);
                Line line = tree.queryLine(xs[x]);
                CHECK(line.k * xs[x] + line.b == best);
            }
        };

        LiChaoTree<Line> mx(xs);
        LiChaoTree<Line, std::less<long long>> mn(xs);
        // Each sentinel is worse than every random line in its own direction.
        std::vector<Item> hi{{0, 4, {0, -1000}}};
        std::vector<Item> lo{{0, 4, {0, 1000}}};
        mx.insert(0, 4, hi.front().line);
        mn.insert(0, 4, lo.front().line);
        auto add = [&](int l, int r, Line line) {
            hi.push_back({l, r, line});
            lo.push_back({l, r, line});
            mx.insert(l, r, line);
            mn.insert(l, r, line);
            check(mx, hi, false);
            check(mn, lo, true);
        };
        add(0, 4, {3, 0});
        add(0, 4, {-3, 0}); // Cross exactly at the middle key.
        add(0, 4, {1, 4});  // Cross between registered keys as well.
        add(1, 3, {0, 70});
        add(1, 3, {0, -70});
        add(0, 0, {0, 90});
        add(0, 0, {0, -90});
        add(4, 4, {0, 95});
        add(4, 4, {0, -95});
        add(2, 2, {0, 100});
        add(2, 2, {0, -100});
        add(2, 1, {0, 1000000}); // Empty ranges must not alter either tree.
        add(2, 1, {0, -1000000});
        for (int it = 0; it < 80; ++it) {
            int l = randomInt(0, 4), r = randomInt(l, 4);
            add(l, r, {randomInt(-20, 20), randomInt(-100, 100)});
        }

        // No zero-valued baseline: both directions must select an actual line.
        LiChaoTree<Line> negative(xs);
        LiChaoTree<Line, std::less<long long>> positive(xs);
        std::vector<Item> neg{{0, 4, {2, -100}}, {1, 3, {-3, -80}}, {4, 4, {0, -1}}};
        std::vector<Item> pos{{0, 4, {2, 100}}, {1, 3, {-3, 80}}, {4, 4, {0, 1}}};
        for (std::size_t i = 0; i < neg.size(); ++i) {
            negative.insert(neg[i].l, neg[i].r, neg[i].line);
            positive.insert(pos[i].l, pos[i].r, pos[i].line);
            check(negative, std::vector<Item>(neg.begin(), neg.begin() + i + 1), false);
            check(positive, std::vector<Item>(pos.begin(), pos.begin() + i + 1), true);
        }

        LiChaoTree<Line> oneMax({7});
        LiChaoTree<Line, std::less<long long>> oneMin({7});
        long long bestMax = std::numeric_limits<long long>::lowest();
        long long bestMin = std::numeric_limits<long long>::max();
        for (Line line : std::vector<Line>{{2, -20}, {-3, 30}, {0, -11}, {0, 17}, {0, 17}}) {
            oneMax.insert(0, 0, line);
            oneMin.insert(0, 0, line);
            bestMax = std::max(bestMax, line.k * 7 + line.b);
            bestMin = std::min(bestMin, line.k * 7 + line.b);
            CHECK(oneMax.query(7) == bestMax);
            CHECK(oneMin.query(7) == bestMin);
        }
    });
    return 0;
}
