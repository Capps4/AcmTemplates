#include "../../../../../src/String/StringF4/StringHash/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <string>
inline constexpr int SingleBase[]{3}, SingleMod[]{101};
inline constexpr int TripleBase[]{3, 5, 7}, TripleMod[]{101, 103, 107};
_strhash::u64 otherHash(std::string_view text);
const int *otherHashBases();
template <class Seq, int D, const int *B, const int *P>
std::array<int, D> reference(const Seq &sequence, int l, int r) {
    std::array<int, D> result{};
    using I = std::decay_t<decltype(sequence[0])>;
    for (int i = l; i < r; ++i)
        for (int k = 0; k < D; ++k) {
            unsigned long long symbol;
            if constexpr (std::is_same_v<I, char> || std::is_same_v<I, unsigned char> ||
                          std::is_same_v<I, signed char>)
                symbol = static_cast<unsigned char>(sequence[i]) + 1;
            else if constexpr (std::is_signed_v<I>) {
                __int128 value = sequence[i];
                value = (value % P[k] + P[k] + 1) % P[k];
                symbol = static_cast<unsigned long long>(value);
            } else {
                auto value = static_cast<unsigned __int128>(sequence[i]);
                symbol = static_cast<unsigned long long>((value % P[k] + 1) % P[k]);
            }
            result[k] = int((static_cast<unsigned __int128>(result[k]) * B[k] + symbol) % P[k]);
        }
    return result;
}
template <class Seq, int D, const int *B, const int *P>
void verify(const Seq &sequence) {
    _strhash::Impl<D, B, P> hash(sequence);
    int n = int(sequence.size());
    CHECK(hash.getArray(n, n) ==
          std::array<int, D>{}); // First query must support the empty interval.
    for (int l = 0; l <= n; ++l)
        for (int r = l; r <= n; ++r) {
            CHECK(hash.getArray(l, r) == reference<Seq, D, B, P>(sequence, l, r));
            if constexpr (D <= 2) {
                auto arr = hash.getArray(l, r);
                _strhash::u64 packed = arr[0];
                if constexpr (D == 2)
                    packed |= _strhash::u64(arr[1]) << 32;
                CHECK(hash.getU64(l, r) == packed);
            }
        }
    if constexpr (D <= 2)
        CHECK(hash.getU64(0) == hash.getU64(0, n));
}
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("StringHash/01-empty", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("");
    });
    runCase("StringHash/02-single", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("a");
    });
    runCase("StringHash/03-overlap", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("aaabaaa");
    });
    runCase("StringHash/04-periodic-tail", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("abcabcab");
    });
    runCase("StringHash/05-many-clones", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("mississippi");
    });
    runCase("StringHash/06-alphabet", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("StringHash/07-nested-palindromes", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("abacabadabacaba");
    });
    runCase("StringHash/08-skewed", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("zzxyzzx");
    });
    runCase("StringHash/09-odd-palindrome", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("abcdefedcba");
    });
    runCase("StringHash/10-even-palindromes", [] {
        verify<std::string, 2, _strhash::b, _strhash::p>("abbaabba");
    });
    return finishCases(10);
}
