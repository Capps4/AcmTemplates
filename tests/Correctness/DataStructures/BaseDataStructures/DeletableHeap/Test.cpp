#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/DataStructures/BaseDataStructures/DeletableHeap/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <algorithm>
#include <iterator>
#include <set>
#include <string>


int coreCases() {
    DeletableHeap<int> heap;
    std::multiset<int> expected;
    for (int step = 0; step < 256; ++step) {
        test_context::step = step;
        int action = randomInt(0, 3);
        if (expected.empty() || action == 0 || expected.size() < 10) {
            int value = randomInt(-100, 100); expected.insert(value); heap.push(value);
        } else if (action == 1 || expected.size() > 300) {
            auto it = std::next(expected.begin(), randomInt(0, int(expected.size()) - 1));
            heap.erase(*it); expected.erase(it);
        } else if (action == 2) {
            CHECK(heap.top() == *expected.rbegin()); heap.pop(); expected.erase(std::prev(expected.end()));
        }
        CHECK(heap.size() == expected.size() && heap.empty() == expected.empty());
        if (!expected.empty()) CHECK(heap.top() == *expected.rbegin());
    }
    auto compare = [reverse = true](int a, int b) { return reverse ? a > b : a < b; };
    std::vector<int> initial{5, 2, 2, 9, -1};
    DeletableHeap minimum(initial, compare);
    minimum.erase(2); initial.erase(std::find(initial.begin(), initial.end(), 2));
    std::sort(initial.begin(), initial.end());
    for (int value : initial) { CHECK(minimum.top() == value); minimum.pop(); }
    CHECK(minimum.empty());
    DeletableHeap<std::string> strings;
    strings.emplace(1000, 'x'); strings.push(std::string(1000, 'y'));
    const auto& top = strings.top(); CHECK(top == std::string(1000, 'y'));
    CHECK(&top == &strings.top()); strings.erase(std::string(1000, 'y')); CHECK(strings.top() == std::string(1000, 'x'));
    std::cout << "50K multiset oracle operations, duplicates, captured comparator/CTAD, bulk build and string emplace passed\n";
    return 0;
}

#include "../../../../../src/DataStructures/BaseDataStructures/DeletableHeap/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
void verifyAdded(const std::vector<int> &a) {
    test_context::describe(a);
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

int run() {
    runCase("DeletableHeap/empty", [] {
        verifyAdded({});
    });
    runCase("DeletableHeap/single", [] {
        verifyAdded({0});
    });
    runCase("DeletableHeap/duplicates", [] {
        verifyAdded({1, 1, 1});
    });
    runCase("DeletableHeap/descending", [] {
        verifyAdded({3, 2, 1});
    });
    runCase("DeletableHeap/mixed-repeated", [] {
        verifyAdded({0, -1, 1});
    });
    runCase("DeletableHeap/duplicate-delayed-deletion", [] {
        verifyAdded({7, 7, 0, 7});
    });
    runCase("DeletableHeap/interleaved", [] {
        verifyAdded({4, 0, -3, 9, 0});
    });
    return 0;
}
}

int main() {
    runCase("DeletableHeap/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
