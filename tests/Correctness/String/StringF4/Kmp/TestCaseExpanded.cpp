#include "../../../../../src/String/StringF4/Kmp/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(std::string_view s) {
    int n = int(s.size());
    std::vector<int> expected(n);
    for (int i = 1; i < n; ++i)
        for (int k = 1; k <= i; ++k)
            if (s.substr(0, k) == s.substr(i - k + 1, k))
                expected[i] = k;
    CHECK(kmp(s) == expected);
}

int main() {
    runCase("Kmp/01-empty", [] {
        verifyAdded("");
    });
    runCase("Kmp/02-single", [] {
        verifyAdded("a");
    });
    runCase("Kmp/03-overlap", [] {
        verifyAdded("aaabaaa");
    });
    runCase("Kmp/04-periodic-tail", [] {
        verifyAdded("abcabcab");
    });
    runCase("Kmp/05-many-clones", [] {
        verifyAdded("mississippi");
    });
    runCase("Kmp/06-alphabet", [] {
        verifyAdded("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("Kmp/07-nested-palindromes", [] {
        verifyAdded("abacabadabacaba");
    });
    runCase("Kmp/08-skewed", [] {
        verifyAdded("zzxyzzx");
    });
    runCase("Kmp/09-odd-palindrome", [] {
        verifyAdded("abcdefedcba");
    });
    runCase("Kmp/10-even-palindromes", [] {
        verifyAdded("abbaabba");
    });
    return finishCases(10);
}
