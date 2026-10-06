#include "../../../../../src/DataStructures/BaseDataStructures/RMQ/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(const std::vector<int> &a) {
    RMQ<int> lo(a);
    RMQ<int, std::greater<int>> hi(a);
    for (int l = 0; l < int(a.size()); ++l)
        for (int r = l + 1; r <= int(a.size()); ++r) {
            CHECK(lo(l, r) == *std::min_element(a.begin() + l, a.begin() + r));
            CHECK(hi(l, r) == *std::max_element(a.begin() + l, a.begin() + r));
        }
}

int main() {
    runCase("RMQ/01-intervals-00", [] {
        verifyAdded({});
    });
    runCase("RMQ/02-intervals-01", [] {
        verifyAdded({-5});
    });
    runCase("RMQ/03-intervals-02", [] {
        verifyAdded({-2, -2, -2});
    });
    runCase("RMQ/04-intervals-03", [] {
        verifyAdded({-5, -4, -3, -2});
    });
    runCase("RMQ/05-intervals-04", [] {
        verifyAdded({-1, -2, -3, -4, -5});
    });
    runCase("RMQ/06-intervals-05", [] {
        verifyAdded({-3, -5, -3, -4, -5});
    });
    runCase("RMQ/07-intervals-06", [] {
        verifyAdded({-5, 995});
    });
    runCase("RMQ/08-intervals-07", [] {
        verifyAdded({4, -5, 4, -5, 4});
    });
    runCase("RMQ/09-intervals-08", [] {
        verifyAdded({-4, -5, -4, -5});
    });
    runCase("RMQ/10-intervals-09", [] {
        verifyAdded({2, -1, 2, -3, -1, -5});
    });
    return finishCases(10);
}
