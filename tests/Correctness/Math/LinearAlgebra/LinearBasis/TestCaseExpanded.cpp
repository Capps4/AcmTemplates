#include "../../../../../src/Math/LinearAlgebra/LinearBasis/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
#include <set>
void verifyAdded(const std::vector<int> &a) {
    LinearBasis<int> b;
    for (int x : a)
        b.insert(x);
    std::set<int> all{0}, nonempty;
    for (int m = 1; m < (1 << a.size()); ++m) {
        int x = 0;
        for (int i = 0; i < int(a.size()); ++i)
            if (m >> i & 1)
                x ^= a[i];
        all.insert(x);
        nonempty.insert(x);
    }
    CHECK(all.size() == (std::size_t(1) << b.rank));
    CHECK(b.canZero == bool(nonempty.count(0)));
    unsigned i = 0;
    for (int x : nonempty)
        CHECK(b.findByOrder(i++) == x);
    for (int x = 0; x < 1024; ++x)
        CHECK(b.check(x) == bool(all.count(x)));
    CHECK(b.getMax() == *all.rbegin());
}

int main() {
    runCase("LinearBasis/01-xor-structure-01", [] {
        verifyAdded({});
    });
    runCase("LinearBasis/02-xor-structure-02", [] {
        verifyAdded({0});
    });
    runCase("LinearBasis/03-xor-structure-03", [] {
        verifyAdded({1});
    });
    runCase("LinearBasis/04-xor-structure-04", [] {
        verifyAdded({1, 1});
    });
    runCase("LinearBasis/05-xor-structure-05", [] {
        verifyAdded({1, 2, 3});
    });
    runCase("LinearBasis/06-xor-structure-06", [] {
        verifyAdded({0, 0, 0});
    });
    runCase("LinearBasis/07-xor-structure-07", [] {
        verifyAdded({1, 2, 4, 8});
    });
    runCase("LinearBasis/08-xor-structure-08", [] {
        verifyAdded({7, 3, 5});
    });
    runCase("LinearBasis/09-xor-structure-09", [] {
        verifyAdded({512, 256, 128});
    });
    runCase("LinearBasis/10-xor-structure-10", [] {
        verifyAdded({1023, 1023, 0, 1});
    });
    return finishCases(10);
}
