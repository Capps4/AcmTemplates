#include "../../../../Support/CaseSupport.hpp"
#include "../../../../Support/TestStack.hpp"
#include <algorithm>
#include "../../../../Support/TestSupport.hpp"
#include "../../../../../src/GraphTheory/Connectivity/TwoSat/code.hpp"

struct Constraint { int x; bool f; int y; bool g; };
bool accepts(const std::vector<Constraint>& constraints, const std::vector<bool>& values) {
    for (auto [x, f, y, g] : constraints) if (values[x] == f && values[y] != g) return false;
    return true;
}
int coreCases() {
    return withTestStack([] {
    TwoSat empty(0);
    CHECK(empty.work() && empty.ans.empty());
    for (int iteration = 0; iteration < 16; ++iteration) {
        test_context::step = iteration;
        int n = randomInt(1, 8);
        TwoSat problem(n);
        std::vector<Constraint> constraints;
        for (int edge = 0, m = randomInt(0, 30); edge < m; ++edge) {
            int x = randomInt(0, n - 1), y = randomInt(0, n - 1);
            bool f = randomInt(0, 1), g = randomInt(0, 1);
            if (randomInt(0, 4) == 0) {
                problem.assign(x, f);
                constraints.push_back({x, !f, x, f});
            } else {
                problem.add(x, f, y, g);
                constraints.push_back({x, f, y, g});
            }
        }
        bool expected = false;
        for (int mask = 0; mask < (1 << n); ++mask) {
            std::vector<bool> values(n);
            for (int i = 0; i < n; ++i) values[i] = mask >> i & 1;
            expected |= accepts(constraints, values);
        }
        for (int repeat = 0; repeat < 2; ++repeat) {
        test_context::step = repeat;
            CHECK(problem.work() == expected);
            if (expected) CHECK(problem.ans.size() == std::size_t(n) && accepts(constraints, problem.ans));
            else CHECK(problem.ans.empty());
        }
    }
    TwoSat changing(1);
    changing.assign(0, true);
    CHECK(changing.work() && changing.ans[0]);
    changing.assign(0, false);
    CHECK(!changing.work() && changing.ans.empty());
    const int n = 128;
    TwoSat chain(n);
    chain.assign(0, true);
    for (int x = 0; x + 1 < n; ++x) chain.add(x, true, x + 1, true);
    CHECK(chain.work() && std::all_of(chain.ans.begin(), chain.ans.end(), [](bool value) { return value; }));
    chain.assign(n - 1, false);
    CHECK(!chain.work() && chain.ans.empty());
    std::cout << "TwoSat exhaustive assignment oracle, repeated work, 200K implication chain PASS\n";
    });
    return 0;
}

#include <algorithm>
#include "../../../../Support/TestSupport.hpp"
#include "../../../../../src/GraphTheory/Connectivity/TwoSat/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
struct Constraint {
    int x;
    bool f;
    int y;
    bool g;
};
bool accepts(const std::vector<Constraint> &constraints, const std::vector<bool> &values) {
    for (auto [x, f, y, g] : constraints)
        if (values[x] == f && values[y] != g)
            return false;
    return true;
}
void verifyAdded(int n, const std::vector<Constraint> &cs) {
    TwoSat t(n);
    for (auto c : cs)
        t.add(c.x, c.f, c.y, c.g);
    bool ok = false;
    for (int mask = 0; mask < (1 << n); ++mask) {
        std::vector<bool> a(n);
        for (int i = 0; i < n; ++i)
            a[i] = mask >> i & 1;
        ok |= accepts(cs, a);
    }
    for (int i = 0; i < 3; ++i) {
        CHECK(t.work() == ok);
        if (ok)
            CHECK(accepts(cs, t.ans));
        else
            CHECK(t.ans.empty());
    }
}

int run() {
    runCase("TwoSat/empty", [] {
        verifyAdded(0, {});
    });
    runCase("TwoSat/free", [] {
        verifyAdded(3, {});
    });
    runCase("TwoSat/force-true", [] {
        verifyAdded(1, {{0, false, 0, true}});
    });
    runCase("TwoSat/force-false", [] {
        verifyAdded(1, {{0, true, 0, false}});
    });
    runCase("TwoSat/contradiction", [] {
        verifyAdded(1, {{0, false, 0, true}, {0, true, 0, false}});
    });
    runCase("TwoSat/equivalence", [] {
        verifyAdded(2, {{0, true, 1, true}, {1, true, 0, true}});
    });
    runCase("TwoSat/opposition", [] {
        verifyAdded(
            2,
            {{0, true, 1, false}, {0, false, 1, true}, {1, true, 0, false}, {1, false, 0, true}});
    });
    runCase("TwoSat/tautology", [] {
        verifyAdded(1, {{0, true, 0, true}});
    });
    runCase("TwoSat/implication-chain", [] {
        verifyAdded(
            4, {{0, false, 0, true}, {0, true, 1, true}, {1, true, 2, true}, {2, true, 3, true}});
    });
    runCase("TwoSat/late-conflict", [] {
        verifyAdded(
            3, {{0, false, 0, true}, {0, true, 1, true}, {1, true, 2, true}, {2, true, 2, false}});
    });
    return 0;
}
}

int main() {
    runCase("TwoSat/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
