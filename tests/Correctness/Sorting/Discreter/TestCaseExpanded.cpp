#include "../../../../src/Sorting/Discreter/code.hpp"
#include "../../../Support/CaseSupport.hpp"
void verifyAdded(const std::vector<int> &a) {
    auto b = a;
    std::sort(b.begin(), b.end());
    b.erase(std::unique(b.begin(), b.end()), b.end());
    auto op = discreteFrom(b);
    auto got = a | op;
    for (int i = 0; i < int(a.size()); ++i)
        CHECK(got[i] == int(std::count_if(b.begin(), b.end(), [&](int v) {
                  return v < a[i];
              })));
    CHECK((a | discreteFrom(std::vector<int>(b))) == got);
}

int main() {
    runCase("Discreter/01-rank-00", [] {
        verifyAdded({});
    });
    runCase("Discreter/02-rank-01", [] {
        verifyAdded({-4});
    });
    runCase("Discreter/03-rank-02", [] {
        verifyAdded({-1, -1, -1});
    });
    runCase("Discreter/04-rank-03", [] {
        verifyAdded({-4, -3, -2, -1});
    });
    runCase("Discreter/05-rank-04", [] {
        verifyAdded({0, -1, -2, -3, -4});
    });
    runCase("Discreter/06-rank-05", [] {
        verifyAdded({-2, -4, -2, -3, -4});
    });
    runCase("Discreter/07-rank-06", [] {
        verifyAdded({-4, 996});
    });
    runCase("Discreter/08-rank-07", [] {
        verifyAdded({5, -4, 5, -4, 5});
    });
    runCase("Discreter/09-rank-08", [] {
        verifyAdded({-3, -4, -3, -4});
    });
    runCase("Discreter/10-rank-09", [] {
        verifyAdded({3, 0, 3, -2, 0, -4});
    });
    return finishCases(10);
}
