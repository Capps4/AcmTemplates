#include "../../../../../src/DataStructures/BaseDataStructures/DeletableHeap/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(const std::vector<int> &a) {
    DeletableHeap<int> d;
    std::multiset<int> e;
    for (int x : a) {
        d.push(x);
        e.insert(x);
        CHECK(d.top() == *e.rbegin());
    }
    for (int i = 0; i < int(a.size()); i += 2) {
        d.erase(a[i]);
        e.erase(e.find(a[i]));
        CHECK(d.size() == e.size());
        if (!e.empty())
            CHECK(d.top() == *e.rbegin());
    }
    d.push(11);
    e.insert(11);
    while (!e.empty()) {
        CHECK(d.top() == *e.rbegin());
        d.pop();
        e.erase(std::prev(e.end()));
    }
    CHECK(d.empty());
}

int main() {
    runCase("DeletableHeap/01-deletions-00", [] {
        verifyAdded({});
    });
    runCase("DeletableHeap/02-deletions-01", [] {
        verifyAdded({0});
    });
    runCase("DeletableHeap/03-deletions-02", [] {
        verifyAdded({1, 1, 1});
    });
    runCase("DeletableHeap/04-deletions-03", [] {
        verifyAdded({1, 2, 3});
    });
    runCase("DeletableHeap/05-deletions-04", [] {
        verifyAdded({3, 2, 1});
    });
    runCase("DeletableHeap/06-deletions-05", [] {
        verifyAdded({0, -1, 1});
    });
    runCase("DeletableHeap/07-deletions-06", [] {
        verifyAdded({7, 7, 0, 7});
    });
    runCase("DeletableHeap/08-deletions-07", [] {
        verifyAdded({2147483647, -2147483648});
    });
    runCase("DeletableHeap/09-deletions-08", [] {
        verifyAdded({2, 1, 2, 3, 1});
    });
    runCase("DeletableHeap/10-deletions-09", [] {
        verifyAdded({4, 0, -3, 9, 0});
    });
    return finishCases(10);
}
