#include "../../../../../src/String/StringF4/ZFunction/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(std::string_view s) {
    int n = int(s.size());
    std::vector<int> expected(n);
    for (int i = 1; i < n; ++i)
        while (i + expected[i] < n and s[expected[i]] == s[i + expected[i]])
            ++expected[i];
    CHECK(zFunction(s) == expected);
}

int main() {
    runCase("ZFunction/01-empty", [] {
        verifyAdded("");
    });
    runCase("ZFunction/02-single", [] {
        verifyAdded("a");
    });
    runCase("ZFunction/03-overlap", [] {
        verifyAdded("aaabaaa");
    });
    runCase("ZFunction/04-periodic-tail", [] {
        verifyAdded("abcabcab");
    });
    runCase("ZFunction/05-many-clones", [] {
        verifyAdded("mississippi");
    });
    runCase("ZFunction/06-alphabet", [] {
        verifyAdded("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("ZFunction/07-nested-palindromes", [] {
        verifyAdded("abacabadabacaba");
    });
    runCase("ZFunction/08-skewed", [] {
        verifyAdded("zzxyzzx");
    });
    runCase("ZFunction/09-odd-palindrome", [] {
        verifyAdded("abcdefedcba");
    });
    runCase("ZFunction/10-even-palindromes", [] {
        verifyAdded("abbaabba");
    });
    return finishCases(10);
}
