#include "Final.hpp"
#include "../TestSupport.hpp"
#include <algorithm>
#include <cstdint>
#include <set>
#include <vector>
int main() {
    LinearBasis<int> empty;
    CHECK(empty.check(0) && !empty.check(1));
    for (int trial = 0; trial < 1000; ++trial) {
        int n = randomInt(0, 11);
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
}
