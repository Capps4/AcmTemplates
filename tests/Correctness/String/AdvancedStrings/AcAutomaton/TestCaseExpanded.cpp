#include "../../../../Support/AutomataSupport.hpp"

int main() {
    runCase("AcAutomaton/01-corpus-01", [] {
        automata_test::acCase<26, 'a'>({}, "");
    });
    runCase("AcAutomaton/02-corpus-02", [] {
        automata_test::acCase<26, 'a'>({""}, "abc");
    });
    runCase("AcAutomaton/03-corpus-03", [] {
        automata_test::acCase<26, 'a'>({"a", "a", "aa"}, "aaaaaa");
    });
    runCase("AcAutomaton/04-corpus-04", [] {
        automata_test::acCase<26, 'a'>({"he", "she", "hers", "his"}, "ahishers");
    });
    runCase("AcAutomaton/05-corpus-05", [] {
        automata_test::acCase<26, 'a'>({"ab", "bc", "abc"}, "abcabcab");
    });
    runCase("AcAutomaton/06-corpus-06", [] {
        automata_test::acCase<26, 'a'>({"abc", "def"}, "abcdef");
    });
    runCase("AcAutomaton/07-corpus-07", [] {
        automata_test::acCase<26, 'a'>({"a", "ab", "abc", "abcd"}, "abcdabcab");
    });
    runCase("AcAutomaton/08-corpus-08", [] {
        automata_test::acCase<26, 'a'>({"", "", "aba"}, "ababa");
    });
    runCase("AcAutomaton/09-corpus-09", [] {
        automata_test::acCase<26, 'a'>({"zzzz", "x"}, "xxxx");
    });
    runCase("AcAutomaton/10-corpus-10", [] {
        automata_test::acCase<26, 'a'>({"abcabc", "bcabc", "cabc"}, "abcabcabc");
    });
    return finishCases(10);
}
