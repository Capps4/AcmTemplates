#include "../../../../../src/DataStructures/TreeDataStructures/SegTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
#include "TestTypes.hpp"
void verifyAdded(int n, int shape) {
    std::vector<long long> a(n);
    std::iota(a.begin(), a.end(), 1);
    auto d = SegTree<SumInfo, AffineTag>(a);
    auto verifyState = [&] {
        for (int l = 0; l < n; ++l)
            for (int r = l + 1; r <= n; ++r)
                CHECK(d.query(l, r).val == std::accumulate(a.begin() + l, a.begin() + r, 0LL));
    };
    verifyState();
    for (int step = 0; step < 12 and n > 0; ++step) {
        int l = shape ? 0 : step % n, r = shape ? n : std::min(n, l + 1 + step % 4);
        AffineTag t{step % 3 == 0 ? -1 : 1, step - 5};
        d.modify(l, r, t);
        for (int i = l; i < r; ++i)
            a[i] = a[i] * t.mul + t.add;
        auto saved = d;
        verifyState();
        for (int i = 0; i < n; ++i)
            CHECK(saved.query(i, i + 1).val == a[i]);
        int p = step % n;
        d.modify(p, SumInfo(step));
        a[p] = step;
        verifyState();
    }
}

int main() {
    runCase("SegTree/01-lazy-00", [] {
        verifyAdded(0, 0);
    });
    runCase("SegTree/02-lazy-01", [] {
        verifyAdded(1, 0);
    });
    runCase("SegTree/03-lazy-02", [] {
        verifyAdded(2, 1);
    });
    runCase("SegTree/04-lazy-03", [] {
        verifyAdded(3, 0);
    });
    runCase("SegTree/05-lazy-04", [] {
        verifyAdded(4, 1);
    });
    runCase("SegTree/06-lazy-05", [] {
        verifyAdded(5, 0);
    });
    runCase("SegTree/07-lazy-06", [] {
        verifyAdded(7, 1);
    });
    runCase("SegTree/08-lazy-07", [] {
        verifyAdded(8, 0);
    });
    runCase("SegTree/09-lazy-08", [] {
        verifyAdded(15, 1);
    });
    runCase("SegTree/10-lazy-09", [] {
        verifyAdded(17, 0);
    });
    return finishCases(10);
}
