#include "../../../../../src/String/AdvancedStrings/SuffixArray/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>
template <class Seq, class Cmp>
void verify(const Seq &s, Cmp cmp) {
    SuffixArray suffixes(s);
    int n = int(s.size());
    std::vector<int> expected(n);
    std::iota(expected.begin(), expected.end(), 0);
    auto equiv = [&](int x, int y) {
        return !cmp(s[x], s[y]) && !cmp(s[y], s[x]);
    };
    std::sort(expected.begin(), expected.end(), [&](int x, int y) {
        while (x < n && y < n && equiv(x, y)) {
            ++x;
            ++y;
        }
        return (x == n && y != n) || (x != n && y != n && cmp(s[x], s[y]));
    });
    CHECK(suffixes.sa == expected);
    CHECK(suffixes.h.size() == s.size());
    for (int rank = 0; rank < n; ++rank) {
        CHECK(suffixes.rk[expected[rank]] == rank);
        int length = 0;
        if (rank)
            while (expected[rank] + length < n && expected[rank - 1] + length < n &&
                   equiv(expected[rank] + length, expected[rank - 1] + length))
                ++length;
        CHECK(suffixes.h[rank] == length);
    }
}
void verifyBytes(const std::string &text) {
    std::vector<unsigned char> bytes(text.begin(), text.end());
    verify(bytes, std::less<>{});
    SuffixArray actual(text), expected(bytes);
    CHECK(actual.sa == expected.sa && actual.rk == expected.rk && actual.h == expected.h);
    verify(std::string_view(text), [](char a, char b) {
        return static_cast<unsigned char>(a) < static_cast<unsigned char>(b);
    });
}
struct Record {
    int key;
    int ignored;
    bool operator<(Record b) const {
        return key < b.key;
    }
    bool operator==(Record b) const {
        return key == b.key;
    }
};
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("SuffixArray/01-empty", [] {
        verifyBytes("");
    });
    runCase("SuffixArray/02-single", [] {
        verifyBytes("a");
    });
    runCase("SuffixArray/03-overlap", [] {
        verifyBytes("aaabaaa");
    });
    runCase("SuffixArray/04-periodic-tail", [] {
        verifyBytes("abcabcab");
    });
    runCase("SuffixArray/05-many-clones", [] {
        verifyBytes("mississippi");
    });
    runCase("SuffixArray/06-alphabet", [] {
        verifyBytes("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("SuffixArray/07-nested-palindromes", [] {
        verifyBytes("abacabadabacaba");
    });
    runCase("SuffixArray/08-skewed", [] {
        verifyBytes("zzxyzzx");
    });
    runCase("SuffixArray/09-odd-palindrome", [] {
        verifyBytes("abcdefedcba");
    });
    runCase("SuffixArray/10-even-palindromes", [] {
        verifyBytes("abbaabba");
    });
    return finishCases(10);
}
