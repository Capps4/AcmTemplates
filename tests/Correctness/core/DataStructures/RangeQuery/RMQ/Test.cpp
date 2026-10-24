#include "../../../../../Support/CaseSupport.hpp"
#include "../../../../../../src/core/DataStructures/RangeQuery/RMQ/code.hpp"
#include "../../../../../Support/TestSupport.hpp"
#include <algorithm>
#include <memory>
#include <numeric>

struct Record {
    int value = 0;
    Record() = default;
    explicit Record(int value) : value(value) {}
};
int coreCases() {
    RMQ<int> empty(std::vector<int>{}, std::min<int>);
    const RMQ gcd(std::vector<int>{12, 18, 9, 15}, std::gcd<int, int>);
    CHECK(gcd(0, 2) == 6);
    CHECK(gcd(1, 4) == 3);
    CHECK(gcd(2, 3) == 9);
    for (int trial = 0; trial < 8; ++trial) {
        test_context::step = trial;
        std::vector<int> a(randomInt(1, 120));
        for (int& value : a)
            value = randomInt(-100, 100);
        test_context::describe(a);
        const RMQ minimum(a, [](int x, int y) { return std::min(x, y); });
        const RMQ maximum(a, [](int x, int y) { return std::max(x, y); });
        auto combine = [mask = std::make_unique<int>(7)](int x, int y) {
            return (y ^ *mask) < (x ^ *mask) ? y : x;
        };
        const RMQ custom(a, std::move(combine));
        for (int l = 0; l < int(a.size()); ++l)
            for (int r = l + 1; r <= int(a.size()); ++r) {
                CHECK(minimum(l, r) == *std::min_element(a.begin() + l, a.begin() + r));
                CHECK(maximum(l, r) == *std::max_element(a.begin() + l, a.begin() + r));
                int expected = a[l];
                for (int i = l + 1; i < r; ++i)
                    if ((a[i] ^ 7) < (expected ^ 7))
                        expected = a[i];
                CHECK(custom(l, r) == expected);
            }
    }
    std::vector<Record> a{Record(4), Record(2), Record(3)};
    const RMQ records(a, [](const Record& x, const Record& y) {
        return x.value < y.value ? x : y;
    });
    static_assert(std::is_same_v<decltype(records(0, 3)), Record>);
    a[1].value = 9;
    CHECK(records(0, 3).value == 2);
    const auto& first = records(0, 3);
    CHECK(first.value == records(0, 3).value);
    auto copy = records(0, 3);
    copy.value = 8;
    CHECK(copy.value == 8 and records(0, 3).value == 2);
    return 0;
}

unsigned gcdOracle(unsigned a, unsigned b) {
    while (b != 0) {
        unsigned remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

void verifyAggregates(const std::vector<unsigned>& a) {
    test_context::describe(a);
    const RMQ gcd(a, [](unsigned x, unsigned y) { return std::gcd(x, y); });
    const RMQ bitsAnd(a, std::bit_and<unsigned>{});
    const RMQ bitsOr(a, std::bit_or<unsigned>{});
    for (int l = 0; l < int(a.size()); ++l) {
        unsigned expectedGcd = 0, expectedAnd = ~0u, expectedOr = 0;
        for (int r = l + 1; r <= int(a.size()); ++r) {
            expectedGcd = gcdOracle(expectedGcd, a[r - 1]);
            expectedAnd &= a[r - 1];
            expectedOr |= a[r - 1];
            CHECK(gcd(l, r) == expectedGcd);
            CHECK(bitsAnd(l, r) == expectedAnd);
            CHECK(bitsOr(l, r) == expectedOr);
        }
    }
}

void verifyOrder(const std::vector<std::pair<int, int>>& a) {
    const RMQ ends(a, [](const auto& x, const auto& y) {
        return std::make_pair(x.first, y.second);
    });
    for (int l = 0; l < int(a.size()); ++l)
        for (int r = l + 1; r <= int(a.size()); ++r)
            CHECK(ends(l, r) == std::make_pair(a[l].first, a[r - 1].second));
}

namespace boundary_cases {
void verifyAdded(const std::vector<int> &a) {
    test_context::describe(a);
    const RMQ lo(a, std::min<int>);
    const RMQ hi(a, std::max<int>);
    for (int l = 0; l < int(a.size()); ++l)
        for (int r = l + 1; r <= int(a.size()); ++r) {
            CHECK(lo(l, r) == *std::min_element(a.begin() + l, a.begin() + r));
            CHECK(hi(l, r) == *std::max_element(a.begin() + l, a.begin() + r));
        }
}

int run() {
    runCase("RMQ/empty", [] {
        verifyAdded({});
    });
    runCase("RMQ/single", [] {
        verifyAdded({-5});
    });
    runCase("RMQ/duplicates", [] {
        verifyAdded({-2, -2, -2});
    });
    runCase("RMQ/descending", [] {
        verifyAdded({-1, -2, -3, -4, -5});
    });
    runCase("RMQ/wide-gap", [] {
        verifyAdded({-5, 995});
    });
    runCase("RMQ/interleaved", [] {
        verifyAdded({2, -1, 2, -3, -1, -5});
    });
    return 0;
}
}

int main() {
    runCase("RMQ/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    runCase("RMQ/gcd-and-bitwise", [] {
        verifyAggregates({});
        verifyAggregates({0, 0, 0});
        verifyAggregates({6, 10, 15, 0, 9, 21});
        verifyAggregates({0, ~0u, 1u << 31, 1, 0xaaaaaaaau, 0x55555555u});
        for (int n : {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65, 127, 128, 129, 193}) {
            test_context::step = n;
            std::vector<unsigned> a(n);
            for (auto& value : a)
                value = unsigned(randomInt(0, 1023));
            verifyAggregates(a);
        }
    });
    runCase("RMQ/noncommutative", [] {
        verifyOrder({});
        for (int n : {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65, 127, 128, 129, 193}) {
            test_context::step = n;
            std::vector<std::pair<int, int>> a(n);
            for (int i = 0; i < n; ++i)
                a[i] = {i, n - i};
            verifyOrder(a);
            for (auto& value : a)
                value = {randomInt(-100, 100), randomInt(-100, 100)};
            verifyOrder(a);
        }
    });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
