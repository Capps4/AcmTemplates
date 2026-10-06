#include "../../../../../src/DataStructures/TreeDataStructures/Trie/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
struct AddedInfo {
    int pass = 0, end = 0;
};
using AddedTrie = StringTrie<AddedInfo>;
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::string> a(n);
    for (auto &s : a) {
        s.resize(12);
        for (auto &c : s)
            c = input.shape ? 'a' : char('a' + random() % 26);
    }
    return measure(input, "string trie insertion/query; random / shared path",
                   [&]() -> std::uint64_t {
                       AddedTrie::clearInit(std::size_t(n) * 13);
                       AddedTrie d;
                       for (auto &s : a)
                           d.modify(s, [&](AddedInfo &i, int dep) {
                               ++i.pass;
                               if (dep == int(s.size()))
                                   ++i.end;
                           });
                       std::uint64_t sum = 0;
                       for (auto &s : a)
                           sum += d.query(s).end;
                       return sum;
                   });
}
