#include "../../../../src/Sorting/MoSort/code.hpp"
#include "../../../Support/CaseSupport.hpp"
void verifyAdded(int n, int shape) {
    Query::rangeScale(n);
    std::vector<Query> q;
    std::vector<int> a(n);
    std::iota(a.begin(), a.end(), 1);
    for (int l = 0; l <= n; ++l)
        for (int r = l; r <= n; ++r)
            if (!shape or l == r or r == n)
                q.emplace_back(l, r, int(q.size()));
    int count = int(q.size());
    std::sort(q.begin(), q.end());
    std::vector<bool> seen(count);
    int l = 0, r = 0;
    long long sum = 0;
    for (auto x : q) {
        CHECK(!seen[x.id]);
        seen[x.id] = true;
        while (l > x.l)
            sum += a[--l];
        while (r < x.r)
            sum += a[r++];
        while (l < x.l)
            sum -= a[l++];
        while (r > x.r)
            sum -= a[--r];
        CHECK(sum == std::accumulate(a.begin() + x.l, a.begin() + x.r, 0LL));
    }
}

int main() {
    runCase("MoSort/01-ranges-00", [] {
        verifyAdded(0, 0);
    });
    runCase("MoSort/02-ranges-01", [] {
        verifyAdded(1, 0);
    });
    runCase("MoSort/03-ranges-02", [] {
        verifyAdded(1, 1);
    });
    runCase("MoSort/04-ranges-03", [] {
        verifyAdded(2, 0);
    });
    runCase("MoSort/05-ranges-04", [] {
        verifyAdded(3, 1);
    });
    runCase("MoSort/06-ranges-05", [] {
        verifyAdded(7, 0);
    });
    runCase("MoSort/07-ranges-06", [] {
        verifyAdded(8, 1);
    });
    runCase("MoSort/08-ranges-07", [] {
        verifyAdded(17, 0);
    });
    runCase("MoSort/09-ranges-08", [] {
        verifyAdded(31, 1);
    });
    runCase("MoSort/10-ranges-09", [] {
        verifyAdded(64, 0);
    });
    return finishCases(10);
}
