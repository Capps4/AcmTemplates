#include "../../../../Support/AutomataSupport.hpp"

int main() {
    runCase("ExSuffixAutomaton/01-corpus-01", [] {
        automata_test::exCase<26, 'a'>({});
    });
    runCase("ExSuffixAutomaton/02-corpus-02", [] {
        automata_test::exCase<26, 'a'>({""});
    });
    runCase("ExSuffixAutomaton/03-corpus-03", [] {
        automata_test::exCase<26, 'a'>({"a", "a", "aa"});
    });
    runCase("ExSuffixAutomaton/04-corpus-04", [] {
        automata_test::exCase<26, 'a'>({"he", "she", "hers", "his"});
    });
    runCase("ExSuffixAutomaton/05-corpus-05", [] {
        automata_test::exCase<26, 'a'>({"ab", "bc", "abc"});
    });
    runCase("ExSuffixAutomaton/06-corpus-06", [] {
        automata_test::exCase<26, 'a'>({"abc", "def"});
    });
    runCase("ExSuffixAutomaton/07-corpus-07", [] {
        automata_test::exCase<26, 'a'>({"a", "ab", "abc", "abcd"});
    });
    runCase("ExSuffixAutomaton/08-corpus-08", [] {
        automata_test::exCase<26, 'a'>({"", "", "aba"});
    });
    runCase("ExSuffixAutomaton/09-corpus-09", [] {
        automata_test::exCase<26, 'a'>({"zzzz", "x"});
    });
    runCase("ExSuffixAutomaton/10-corpus-10", [] {
        automata_test::exCase<26, 'a'>({"abcabc", "bcabc", "cabc"});
    });
    return finishCases(10);
}
