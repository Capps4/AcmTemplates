#include "../../../../../src/DataStructures/BaseDataStructures/DeletableHeap/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <algorithm>
#include <iterator>
#include <set>
#include <string>

int main() {
    DeletableHeap<int> heap;
    std::multiset<int> expected;
    for (int step = 0; step < 50000; ++step) {
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
}
