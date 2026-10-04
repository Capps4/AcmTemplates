#include "Final.hpp"
#include "../TestSupport.hpp"
#include <cstdint>
#include <map>
#include <tuple>

struct InputEdge { int x, y, cap, fee; };
std::map<int, long long> enumerate(int n, const std::vector<InputEdge>& edges) {
    std::vector<int> balance(n);
    std::map<int, long long> best;
    auto visit = [&](auto&& self, std::size_t i, long long cost) -> void {
        if (i == edges.size()) {
            if (balance[0] < 0 || balance[n - 1] != -balance[0]) return;
            for (int x = 1; x + 1 < n; ++x) if (balance[x] != 0) return;
            auto [it, inserted] = best.emplace(balance[0], cost);
            if (!inserted) it->second = std::min(it->second, cost);
            return;
        }
        const auto& arc = edges[i];
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
void verifyMode(int n, const std::vector<InputEdge>& edges, const std::map<int, long long>& best, int limit) {
    CostFlow<int, long long> solver(n);
    for (const auto& arc : edges) solver.add(arc.x, arc.y, arc.cap, arc.fee);
    std::pair<int, long long> expected{0, best.at(0)};
    for (auto [flow, cost] : best) {
        if (flow > limit) continue;
        if constexpr (mode == MAX_FLOW_MIN_COST) {
            if (flow > expected.first) expected = {flow, cost};
        } else {
            bool tie = mode == MIN_COST_MAX_FLOW ? flow > expected.first : flow < expected.first;
            if (cost < expected.second || (cost == expected.second && tie)) expected = {flow, cost};
        }
    }
    CHECK(solver.template work<mode>(0, n - 1, limit) == expected);
    std::vector<int> balance(n);
    long long cost = 0;
    for (std::size_t i = 0; i < edges.size(); ++i) {
        const auto& arc = edges[i];
        int used = arc.cap - solver.edge[2 * i].cap;
        CHECK(0 <= used && used <= arc.cap);
        CHECK(solver.edge[2 * i + 1].cap == used);
        balance[arc.x] += used;
        balance[arc.y] -= used;
        cost += 1LL * used * arc.fee;
    }
    CHECK(cost == expected.second && balance[0] == expected.first && balance[n - 1] == -expected.first);
    for (int x = 1; x + 1 < n; ++x) CHECK(balance[x] == 0);
}

int main() {
    for (int iteration = 0; iteration < 5000; ++iteration) {
        int n = randomInt(2, 5), m = randomInt(0, 7);
        std::vector<InputEdge> edges;
        std::vector<int> potential(n);
        for (int& value : potential) value = randomInt(-5, 5);
        for (int i = 0; i < m; ++i) {
            int x = randomInt(0, n - 1), y = randomInt(0, n - 1);
            int fee;
            if (iteration % 2) {
                if (x == y) continue;
                if (x > y) std::swap(x, y);
                fee = randomInt(-5, 5); // Arbitrary signed fees on a DAG.
            } else {
                fee = randomInt(0, 5) + potential[y] - potential[x]; // All cycles nonnegative.
            }
            edges.push_back({x, y, randomInt(0, 2), fee});
        }
        auto best = enumerate(n, edges);
        int limit = randomInt(0, 8);
        verifyMode<MAX_FLOW_MIN_COST>(n, edges, best, limit);
        verifyMode<MIN_COST_MAX_FLOW>(n, edges, best, limit);
        verifyMode<MIN_COST_MIN_FLOW>(n, edges, best, limit);
        CostFlow<int, long long> split(n);
        for (const auto& arc : edges) split.add(arc.x, arc.y, arc.cap, arc.fee);
        auto a = split.work(0, n - 1, 1);
        auto b = split.work(0, n - 1);
        CHECK(a.first + b.first == best.rbegin()->first);
        CHECK(a.second + b.second == best.rbegin()->second);
        CHECK(split.work(0, n - 1) == std::pair<int, long long>(0, 0));
    }
    // Rerouting uses reverse arcs: cheapest first assignment must be undone.
    CostFlow<int, int> assignment(6);
    assignment.add(0, 1, 1, 0); assignment.add(0, 2, 1, 0);
    assignment.add(1, 3, 1, 0); assignment.add(1, 4, 1, 1);
    assignment.add(2, 3, 1, 1); assignment.add(2, 4, 1, 100);
    assignment.add(3, 5, 1, 0); assignment.add(4, 5, 1, 0);
    CHECK(assignment.work(0, 5, 1) == std::pair<int, int>(1, 0));
    CHECK(assignment.work(0, 5) == std::pair<int, int>(1, 2));
    // Adding a cheaper route after old flow could introduce a negative residual
    // cycle; this added route preserves the no-negative-cycle contract.
    assignment.add(0, 5, 2, 5);
    CHECK(assignment.work(0, 5) == std::pair<int, int>(2, 10));

    CostFlow<int, long long> large(3);
    auto maxFee = std::numeric_limits<long long>::max();
    large.add(0, 1, 1, maxFee); large.add(1, 2, 1, -maxFee);
    CHECK(large.work(0, 2) == std::pair<int, long long>(1, 0));
    CostFlow<std::uint64_t, long long> huge(2);
    auto maxCap = std::numeric_limits<std::uint64_t>::max();
    huge.add(0, 1, maxCap, 0); huge.add(0, 1, maxCap, 0);
    CHECK(huge.work(0, 1).first == maxCap);
    CHECK(huge.work(0, 1).first == maxCap);
    CostFlow<int, int> overflow(2);
    overflow.add(0, 1, 2, std::numeric_limits<int>::max());
    bool caught = false;
    try { overflow.work(0, 1); } catch (const std::overflow_error&) { caught = true; }
    CHECK(caught && overflow.edge[0].cap == 2);
    CHECK(overflow.work(0, 1, 1) == std::pair<int, int>(1, std::numeric_limits<int>::max()));
    CostFlow<std::uint64_t, long long> productOverflow(4);
    productOverflow.add(0, 1, maxCap, maxFee);
    productOverflow.add(1, 2, maxCap, maxFee);
    productOverflow.add(2, 3, maxCap, maxFee);
    caught = false;
    try { productOverflow.work(0, 3); } catch (const std::overflow_error&) { caught = true; }
    CHECK(caught && productOverflow.edge[0].cap == maxCap);

    CostFlow<int, int> cycle(3);
    cycle.add(0, 1, 1, 0); cycle.add(1, 1, 1, -1); cycle.add(1, 2, 1, 0);
    caught = false;
    try { cycle.work(0, 2); } catch (const std::domain_error&) { caught = true; }
    CHECK(caught);
    CostFlow<int, int> unreachableCycle(4);
    unreachableCycle.add(0, 3, 1, 7); unreachableCycle.add(1, 2, 1, -2); unreachableCycle.add(2, 1, 1, 1);
    CHECK(unreachableCycle.work(0, 3) == std::pair<int, int>(1, 7));
    CHECK(cycle.work(0, 0) == std::pair<int, int>(0, 0));
    CHECK(cycle.work(0, 2, 0) == std::pair<int, int>(0, 0));
    CostFlow<int, int> empty(0);
    CHECK(empty.adj.empty());
    CostFlow<int, int> chain(200000);
    for (int x = 1; x < chain.n; ++x) chain.add(x - 1, x, 1, -1);
    CHECK(chain.work(0, chain.n - 1) == std::pair<int, int>(1, -(chain.n - 1)));
    std::cout << "CostFlow 5000 exhaustive-flow graph oracles, 3 modes, limits/repeated/rerouting, numeric errors, negative cycles, 200K chain PASS\n";
}
