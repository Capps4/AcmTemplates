#include "../../../../Support/AutomataSupport.hpp"

int main() {
    runCase("PalindromeAutomaton/01-empty", [] {
        automata_test::pamCase<26, 'a'>("");
    });
    runCase("PalindromeAutomaton/02-single", [] {
        automata_test::pamCase<26, 'a'>("a");
    });
    runCase("PalindromeAutomaton/03-overlap", [] {
        automata_test::pamCase<26, 'a'>("aaabaaa");
    });
    runCase("PalindromeAutomaton/04-periodic-tail", [] {
        automata_test::pamCase<26, 'a'>("abcabcab");
    });
    runCase("PalindromeAutomaton/05-repeated-letters", [] {
        automata_test::pamCase<26, 'a'>("mississippi");
    });
    runCase("PalindromeAutomaton/06-alphabet", [] {
        automata_test::pamCase<26, 'a'>("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("PalindromeAutomaton/07-nested-palindromes", [] {
        automata_test::pamCase<26, 'a'>("abacabadabacaba");
    });
    runCase("PalindromeAutomaton/08-skewed", [] {
        automata_test::pamCase<26, 'a'>("zzxyzzx");
    });
    runCase("PalindromeAutomaton/09-odd-palindrome", [] {
        automata_test::pamCase<26, 'a'>("abcdefedcba");
    });
    runCase("PalindromeAutomaton/10-even-palindromes", [] {
        automata_test::pamCase<26, 'a'>("abbaabba");
    });
    return finishCases(10);
}
