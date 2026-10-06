#include "../../../../../src/DataStructures/TreeDataStructures/Trie/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
struct AddedInfo {
    int pass = 0, end = 0;
};
using AddedTrie = StringTrie<AddedInfo>;
void verifyAdded(const std::vector<std::string> &words) {
    AddedTrie::clearInit();
    AddedTrie d;
    auto verify = [&](const AddedTrie &t, const std::vector<std::string> &ws) {
        std::set<std::string> queries{"", "a", "z", "missing"};
        for (auto &s : ws)
            for (std::size_t k = 0; k <= s.size(); ++k)
                queries.insert(s.substr(0, k));
        for (auto &q : queries) {
            int pass = 0, end = 0;
            for (auto &s : ws) {
                pass += s.substr(0, q.size()) == q;
                end += s == q;
            }
            auto got = t.query(q);
            CHECK(got.pass == pass);
            CHECK(got.end == end);
        }
    };
    std::vector<std::string> seen;
    for (auto &s : words) {
        auto old = d;
        auto snapshot = seen;
        d.modify(s, [&](AddedInfo &i, int dep) {
            ++i.pass;
            if (dep == int(s.size()))
                ++i.end;
        });
        seen.push_back(s);
        verify(d, seen);
        verify(old, snapshot);
    }
    verify(d, seen);
}

int main() {
    runCase("Trie/01-words-00", [] {
        verifyAdded({});
    });
    runCase("Trie/02-words-01", [] {
        verifyAdded({""});
    });
    runCase("Trie/03-words-02", [] {
        verifyAdded({"a"});
    });
    runCase("Trie/04-words-03", [] {
        verifyAdded({"a", "a", "aa"});
    });
    runCase("Trie/05-words-04", [] {
        verifyAdded({"ab", "abc", "abd"});
    });
    runCase("Trie/06-words-05", [] {
        verifyAdded({"z", "zz", "zzz"});
    });
    runCase("Trie/07-words-06", [] {
        verifyAdded({"abc", "bca", "cab"});
    });
    runCase("Trie/08-words-07", [] {
        verifyAdded({"", "a", ""});
    });
    runCase("Trie/09-words-08", [] {
        verifyAdded({"ababa", "aba", "ba"});
    });
    runCase("Trie/10-words-09", [] {
        verifyAdded({"apple", "app", "bat", "cat"});
    });
    return finishCases(10);
}
