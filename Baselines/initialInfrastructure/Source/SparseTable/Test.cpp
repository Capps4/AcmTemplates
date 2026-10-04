#include "Final.hpp"
#include "../TestSupport.hpp"
#include <algorithm>
#include <memory>

struct NonDefault {
    int value;
    NonDefault() = default;
    explicit NonDefault(int value) : value(value) {}
    bool operator<(const NonDefault& other) const { return value < other.value; }
};
int main() {
    RMQ<int> empty(std::vector<int>{});
    for (int trial = 0; trial < 200; ++trial) {
        std::vector<int> a(randomInt(1, 120));
        for (int& value : a) value = randomInt(-100, 100);
        RMQ minimum(a);
        RMQ maximum(a, std::greater<>{});
        auto compare = [mask = std::make_unique<int>(7)](int x, int y) { return (x ^ *mask) < (y ^ *mask); };
        RMQ custom(a, std::move(compare));
        for (int l = 0; l < int(a.size()); ++l) for (int r = l + 1; r <= int(a.size()); ++r) {
            CHECK(minimum(l, r) == *std::min_element(a.begin() + l, a.begin() + r));
            CHECK(maximum(l, r) == *std::max_element(a.begin() + l, a.begin() + r));
            int expected = a[l];
            for (int i = l + 1; i < r; ++i) if ((a[i] ^ 7) < (expected ^ 7)) expected = a[i];
            CHECK(custom(l, r) == expected);
        }
    }
    RMQ records(std::vector<NonDefault>{NonDefault(4), NonDefault(2), NonDefault(3)});
    CHECK(records(0, 3).value == 2);
    const auto& first = records(0, 3);
    CHECK(first.value == records(0, 3).value);
    std::cout << "All random intervals vs min/max oracles, move-only comparator, empty table and record values passed\n";
}
