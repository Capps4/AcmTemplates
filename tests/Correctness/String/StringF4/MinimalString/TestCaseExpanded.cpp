#include "../../../../../src/String/StringF4/MinimalString/code.hpp"
#include "../../../../Support/TestSupport.hpp"
template <class Sequence, class Less>
auto naive(const Sequence &input, Less less) {
    auto expected = input;
    for (std::size_t i = 1; i < input.size(); ++i) {
        auto candidate = input;
        std::rotate(candidate.begin(), candidate.begin() + i, candidate.end());
        if (std::lexicographical_compare(candidate.begin(), candidate.end(), expected.begin(),
                                         expected.end(), less))
            expected = std::move(candidate);
    }
    return expected;
}
void verify(const std::string &input) {
    auto expected = naive(input, [](char a, char b) {
        return static_cast<unsigned char>(a) < static_cast<unsigned char>(b);
    });
    CHECK((input | minimalString()) == expected);
    CHECK((std::string_view(input) | minimalString()) == expected);
    CHECK((std::string(input) | minimalString()) == expected);
}
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("MinimalString/01-empty", [] {
        verify("");
    });
    runCase("MinimalString/02-single", [] {
        verify("a");
    });
    runCase("MinimalString/03-overlap", [] {
        verify("aaabaaa");
    });
    runCase("MinimalString/04-periodic-tail", [] {
        verify("abcabcab");
    });
    runCase("MinimalString/05-many-clones", [] {
        verify("mississippi");
    });
    runCase("MinimalString/06-alphabet", [] {
        verify("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("MinimalString/07-nested-palindromes", [] {
        verify("abacabadabacaba");
    });
    runCase("MinimalString/08-skewed", [] {
        verify("zzxyzzx");
    });
    runCase("MinimalString/09-odd-palindrome", [] {
        verify("abcdefedcba");
    });
    runCase("MinimalString/10-even-palindromes", [] {
        verify("abbaabba");
    });
    return finishCases(10);
}
