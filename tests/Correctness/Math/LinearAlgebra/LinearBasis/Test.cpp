#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/LinearAlgebra/LinearBasis/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <algorithm>
#include <cstdint>
#include <set>
#include <vector>

int coreCases() {
    LinearBasis<int> empty;
    CHECK(empty.check(0) && !empty.check(1));
    for (int trial = 0; trial < 12; ++trial) {
        test_context::step = trial;
        int n = randomInt(0, 7);
        std::vector<int> input;
        LinearBasis<int> basis;
        for (int i = 0; i < n; ++i) {
            input.push_back(randomInt(0, 1023));
            basis.insert(input.back());
            if (basis.rank) basis.findByOrder(0); // Reduce before later insertions.
        }
        std::set<int> all{0}, nonempty;
        for (int mask = 1; mask < (1 << n); ++mask) {
            int result = 0;
            for (int i = 0; i < n; ++i) if (mask >> i & 1) result ^= input[i];
            all.insert(result); nonempty.insert(result);
        }
        CHECK(all.size() == (std::size_t(1) << basis.rank));
        CHECK(basis.canZero == (nonempty.count(0) != 0));
        unsigned index = 0;
        for (int value : nonempty) {
            CHECK(basis.findByOrder(index) == value); ++index;
        }
        CHECK(!basis.check(-1));
        for (int query = 0; query < 20; ++query) {
        test_context::step = query;
            int on = randomInt(0, 1023), lo = 1024, hi = 0;
            for (int value : all) { lo = std::min(lo, value ^ on); hi = std::max(hi, value ^ on); }
            CHECK(basis.getMin(on) == lo && basis.getMax(on) == hi && basis.check(on) == bool(all.count(on)));
        }
        basis.clear(); CHECK(basis.rank == 0 && !basis.canZero);
    }
    using U = std::uint64_t;
    constexpr U max = std::numeric_limits<U>::max();
    LinearBasis<U> full;
    for (int i = 0; i < 64; ++i) full.insert(U(1) << i);
    CHECK(full.rank == 64 && full.getMax() == max && full.getMin(max) == 0 && full.check(max));
    CHECK(full.findByOrder(max - 1) == max);
    full.insert(0);
    CHECK(full.canZero && full.findByOrder(max) == max);
    LinearBasis<long long> signedFull;
    for (int i = 0; i < 63; ++i) signedFull.insert(1LL << i);
    CHECK(signedFull.getMax() == std::numeric_limits<long long>::max());
    CHECK(signedFull.findByOrder((U(1) << 63) - 2) == std::numeric_limits<long long>::max());
    LinearBasis<unsigned char> byte;
    for (int i = 0; i < 8; ++i) byte.insert(static_cast<unsigned char>(1u << i));
    CHECK(byte.findByOrder(254) == 255);
    std::cout << "LinearBasis exhaustive subset XOR oracle, reduced-then-insert, empty, full 64-bit rank/kth limits PASS\n";
    return 0;
}

#include "../../../../../src/Math/LinearAlgebra/LinearBasis/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
#include <set>

namespace boundary_cases {
void verifyAdded(const std::vector<int> &a) {
    test_context::describe(a);
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

int run() {
    runCase("LinearBasis/independent-span", [] { verifyAdded({1, 2, 4, 8}); });
    runCase("LinearBasis/empty", [] {
        verifyAdded({});
    });
    runCase("LinearBasis/zero-only", [] {
        verifyAdded({0});
    });
    runCase("LinearBasis/single-bit", [] {
        verifyAdded({1});
    });
    runCase("LinearBasis/dependent-span", [] {
        verifyAdded({1, 2, 3});
    });
    runCase("LinearBasis/duplicates-and-zero", [] {
        verifyAdded({1023, 1023, 0, 1});
    });
    return 0;
}
}

int main() {
    runCase("LinearBasis/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
