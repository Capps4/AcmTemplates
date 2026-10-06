#include "../../../../../src/GraphTheory/Flow/CostFlow/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <cstdint>
#include <map>
#include <tuple>

struct InputEdge {
    int x, y, cap, fee;
};
std::map<int, long long> enumerate(int n, const std::vector<InputEdge> &edges) {
    std::vector<int> balance(n);
    std::map<int, long long> best;
    auto visit = [&](auto &&self, std::size_t i, long long cost) -> void {
        if (i == edges.size()) {
            if (balance[0] < 0 || balance[n - 1] != -balance[0])
                return;
            for (int x = 1; x + 1 < n; ++x)
                if (balance[x] != 0)
                    return;
            auto [it, inserted] = best.emplace(balance[0], cost);
            if (!inserted)
                it->second = std::min(it->second, cost);
            return;
        }
        const auto &arc = edges[i];
        for (int amount = 0; amount <= arc.cap; ++amount) {
            balance[arc.x] += amount;
            balance[arc.y] -= amount;
            self(self, i + 1, cost + amount * arc.fee);
            balance[arc.x] -= amount;
            balance[arc.y] += amount;
        }
    };
    visit(visit, 0, 0);
    return best;
}

template <CostFlowMode mode>
void verifyMode(int n, const std::vector<InputEdge> &edges, const std::map<int, long long> &best,
                int limit) {
    CostFlow<int, long long> solver(n);
    for (const auto &arc : edges)
        solver.add(arc.x, arc.y, arc.cap, arc.fee);
    std::pair<int, long long> expected{0, best.at(0)};
    for (auto [flow, cost] : best) {
        if (flow > limit)
            continue;
        if constexpr (mode == MAX_FLOW_MIN_COST) {
            if (flow > expected.first)
                expected = {flow, cost};
        } else {
            bool tie = mode == MIN_COST_MAX_FLOW ? flow > expected.first : flow < expected.first;
            if (cost < expected.second || (cost == expected.second && tie))
                expected = {flow, cost};
        }
    }
    CHECK(solver.template work<mode>(0, n - 1, limit) == expected);
    std::vector<int> balance(n);
    long long cost = 0;
    for (std::size_t i = 0; i < edges.size(); ++i) {
        const auto &arc = edges[i];
        int used = arc.cap - solver.edge[2 * i].cap;
        CHECK(0 <= used && used <= arc.cap);
        CHECK(solver.edge[2 * i + 1].cap == used);
        balance[arc.x] += used;
        balance[arc.y] -= used;
        cost += 1LL * used * arc.fee;
    }
    CHECK(cost == expected.second && balance[0] == expected.first &&
          balance[n - 1] == -expected.first);
    for (int x = 1; x + 1 < n; ++x)
        CHECK(balance[x] == 0);
}

#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("CostFlow/01-empty", [] {
        std::vector<InputEdge> es{};
        auto best = enumerate(3, es);
        verifyMode<MAX_FLOW_MIN_COST>(3, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(3, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(3, es, best, 3);
    });
    runCase("CostFlow/02-zero-capacity", [] {
        std::vector<InputEdge> es{{0, 1, 0, -3}};
        auto best = enumerate(2, es);
        verifyMode<MAX_FLOW_MIN_COST>(2, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(2, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(2, es, best, 3);
    });
    runCase("CostFlow/03-single-negative", [] {
        std::vector<InputEdge> es{{0, 1, 2, -5}};
        auto best = enumerate(2, es);
        verifyMode<MAX_FLOW_MIN_COST>(2, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(2, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(2, es, best, 3);
    });
    runCase("CostFlow/04-positive-cost", [] {
        std::vector<InputEdge> es{{0, 1, 2, 7}};
        auto best = enumerate(2, es);
        verifyMode<MAX_FLOW_MIN_COST>(2, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(2, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(2, es, best, 3);
    });
    runCase("CostFlow/05-zero-cost", [] {
        std::vector<InputEdge> es{{0, 1, 2, 0}};
        auto best = enumerate(2, es);
        verifyMode<MAX_FLOW_MIN_COST>(2, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(2, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(2, es, best, 3);
    });
    runCase("CostFlow/06-parallel", [] {
        std::vector<InputEdge> es{{0, 1, 1, -2}, {0, 1, 2, 3}};
        auto best = enumerate(2, es);
        verifyMode<MAX_FLOW_MIN_COST>(2, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(2, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(2, es, best, 3);
    });
    runCase("CostFlow/07-negative-dag", [] {
        std::vector<InputEdge> es{{0, 1, 2, -3}, {1, 2, 2, 1}};
        auto best = enumerate(3, es);
        verifyMode<MAX_FLOW_MIN_COST>(3, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(3, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(3, es, best, 3);
    });
    runCase("CostFlow/08-two-paths", [] {
        std::vector<InputEdge> es{{0, 1, 1, 2}, {1, 3, 1, -1}, {0, 2, 1, 0}, {2, 3, 1, 3}};
        auto best = enumerate(4, es);
        verifyMode<MAX_FLOW_MIN_COST>(4, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(4, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(4, es, best, 3);
    });
    runCase("CostFlow/09-zero-self-loop", [] {
        std::vector<InputEdge> es{{0, 0, 1, 0}, {0, 1, 1, 2}};
        auto best = enumerate(2, es);
        verifyMode<MAX_FLOW_MIN_COST>(2, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(2, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(2, es, best, 3);
    });
    runCase("CostFlow/10-reroute", [] {
        std::vector<InputEdge> es{{0, 1, 1, 0}, {0, 2, 1, 0}, {1, 3, 1, 0}, {1, 4, 1, 1},
                                  {2, 3, 1, 1}, {2, 4, 1, 9}, {3, 5, 1, 0}, {4, 5, 1, 0}};
        auto best = enumerate(6, es);
        verifyMode<MAX_FLOW_MIN_COST>(6, es, best, 3);
        verifyMode<MIN_COST_MAX_FLOW>(6, es, best, 3);
        verifyMode<MIN_COST_MIN_FLOW>(6, es, best, 3);
    });
    return finishCases(10);
}
