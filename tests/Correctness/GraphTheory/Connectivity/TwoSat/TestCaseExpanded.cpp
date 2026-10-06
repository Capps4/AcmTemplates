#include <algorithm>
#include "../../../../Support/TestSupport.hpp"
#include "../../../../../src/GraphTheory/Connectivity/TwoSat/code.hpp"
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
#include "../../../../Support/CaseSupport.hpp"
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

int main() {
    runCase("TwoSat/01-empty", [] {
        verifyAdded(0, {});
    });
    runCase("TwoSat/02-free", [] {
        verifyAdded(3, {});
    });
    runCase("TwoSat/03-force-true", [] {
        verifyAdded(1, {{0, false, 0, true}});
    });
    runCase("TwoSat/04-force-false", [] {
        verifyAdded(1, {{0, true, 0, false}});
    });
    runCase("TwoSat/05-contradiction", [] {
        verifyAdded(1, {{0, false, 0, true}, {0, true, 0, false}});
    });
    runCase("TwoSat/06-equivalence", [] {
        verifyAdded(2, {{0, true, 1, true}, {1, true, 0, true}});
    });
    runCase("TwoSat/07-opposition", [] {
        verifyAdded(
            2,
            {{0, true, 1, false}, {0, false, 1, true}, {1, true, 0, false}, {1, false, 0, true}});
    });
    runCase("TwoSat/08-tautology", [] {
        verifyAdded(1, {{0, true, 0, true}});
    });
    runCase("TwoSat/09-implication-chain", [] {
        verifyAdded(
            4, {{0, false, 0, true}, {0, true, 1, true}, {1, true, 2, true}, {2, true, 3, true}});
    });
    runCase("TwoSat/10-late-conflict", [] {
        verifyAdded(
            3, {{0, false, 0, true}, {0, true, 1, true}, {1, true, 2, true}, {2, true, 2, false}});
    });
    return finishCases(10);
}
