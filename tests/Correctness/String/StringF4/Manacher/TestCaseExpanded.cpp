#include "../../../../../src/String/StringF4/Manacher/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <string>
bool palindrome(std::string_view text, int l, int r) {
    while (l < --r)
        if (text[l++] != text[r])
            return false;
    return true;
}
void verify(std::string_view text) {
    Manacher engine(text);
    int n = int(text.size());
    for (int i = 0; i < n; ++i) {
        int odd = 1, even = 0, longest = 0;
        for (int k = 1; k <= i && i + k < n && text[i - k] == text[i + k]; ++k)
            odd += 2;
        if (i + 1 < n) {
            for (int k = 0; k <= i && i + k + 1 < n && text[i - k] == text[i + k + 1]; ++k)
                even += 2;
            CHECK(engine.getPalinLenFromCenter(i, 1) == even);
        }
        CHECK(engine.getPalinLenFromCenter(i, 0) == odd);
        for (int l = 0; l <= i; ++l)
            if (palindrome(text, l, i + 1))
                longest = std::max(longest, i + 1 - l);
        CHECK(engine.getPalinLenFromTail(i) == longest);
    }
    for (int l = 0; l <= n; ++l)
        for (int r = l; r <= n; ++r)
            CHECK(engine.isPalindrome(l, r) == palindrome(text, l, r));
}
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("Manacher/01-empty", [] {
        verify("");
    });
    runCase("Manacher/02-single", [] {
        verify("a");
    });
    runCase("Manacher/03-overlap", [] {
        verify("aaabaaa");
    });
    runCase("Manacher/04-periodic-tail", [] {
        verify("abcabcab");
    });
    runCase("Manacher/05-many-clones", [] {
        verify("mississippi");
    });
    runCase("Manacher/06-alphabet", [] {
        verify("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("Manacher/07-nested-palindromes", [] {
        verify("abacabadabacaba");
    });
    runCase("Manacher/08-skewed", [] {
        verify("zzxyzzx");
    });
    runCase("Manacher/09-odd-palindrome", [] {
        verify("abcdefedcba");
    });
    runCase("Manacher/10-even-palindromes", [] {
        verify("abbaabba");
    });
    return finishCases(10);
}
