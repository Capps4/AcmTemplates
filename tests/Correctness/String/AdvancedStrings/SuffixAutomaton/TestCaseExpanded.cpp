#include "../../../../Support/AutomataSupport.hpp"

int main() {
    runCase("SuffixAutomaton/01-empty", [] {
        automata_test::samCase<26, 'a'>("");
    });
    runCase("SuffixAutomaton/02-single", [] {
        automata_test::samCase<26, 'a'>("a");
    });
    runCase("SuffixAutomaton/03-overlap", [] {
        automata_test::samCase<26, 'a'>("aaabaaa");
    });
    runCase("SuffixAutomaton/04-periodic-tail", [] {
        automata_test::samCase<26, 'a'>("abcabcab");
    });
    runCase("SuffixAutomaton/05-many-clones", [] {
        automata_test::samCase<26, 'a'>("mississippi");
    });
    runCase("SuffixAutomaton/06-alphabet", [] {
        automata_test::samCase<26, 'a'>("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("SuffixAutomaton/07-nested-palindromes", [] {
        automata_test::samCase<26, 'a'>("abacabadabacaba");
    });
    runCase("SuffixAutomaton/08-skewed", [] {
        automata_test::samCase<26, 'a'>("zzxyzzx");
    });
    runCase("SuffixAutomaton/09-odd-palindrome", [] {
        automata_test::samCase<26, 'a'>("abcdefedcba");
    });
    runCase("SuffixAutomaton/10-even-palindromes", [] {
        automata_test::samCase<26, 'a'>("abbaabba");
    });
    return finishCases(10);
}
