#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/String/StringF4/ZFunction/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <string>


int coreCases() {
    auto verify = [](std::string_view s) {
        int n = int(s.size());
        std::vector<int> expected(n);
        for (int i = 1; i < n; ++i)
            while (i + expected[i] < n && s[expected[i]] == s[i + expected[i]]) ++expected[i];
        CHECK(zFunction(s) == expected);
    };
    verify({});
    verify(std::string_view("\0ab\0ab", 6));
    for (int n = 0; n <= 5; ++n) {
        for (int mask = 0; mask < (1 << n); ++mask) {
            std::string s(n, 'a');
            for (int i = 0; i < n; ++i) s[i] += (mask >> i) & 1;
            verify(s);
        }
    }
    for (int trial = 0; trial < 16; ++trial) {
        test_context::step = trial;
        std::string text(randomInt(0, 100), 'a');
        for (auto& c : text) c = char(randomInt(0, 5));
        int l = randomInt(0, int(text.size())), r = randomInt(l, int(text.size()));
        verify(std::string_view(text).substr(l, r - l));
    }
    std::string equal(4096, 'a');
    auto result = zFunction(equal);
    for (int i = 1; i < int(equal.size()); ++i)
        CHECK(result[i] == int(equal.size()) - i);
    std::cout << "Exhaustive binary strings, random slices, embedded NUL and 1M repeated characters passed\n";
    return 0;
}

#include "../../../../../src/String/StringF4/ZFunction/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
void verifyAdded(std::string_view s) {
    test_context::describe(s);
    int n = int(s.size());
    std::vector<int> expected(n);
    for (int i = 1; i < n; ++i)
        while (i + expected[i] < n and s[expected[i]] == s[i + expected[i]])
            ++expected[i];
    CHECK(zFunction(s) == expected);
}

int run() {
    runCase("ZFunction/overlap", [] {
        verifyAdded("aaabaaa");
    });
    runCase("ZFunction/periodic-tail", [] {
        verifyAdded("abcabcab");
    });
    runCase("ZFunction/alphabet", [] {
        verifyAdded("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("ZFunction/even-palindromes", [] {
        verifyAdded("abbaabba");
    });
    runCase("ZFunction/byte-domain", [] {
        verifyAdded(std::string("\0\xff\x80\0", 4));
    });
    return 0;
}
}

int main() {
    runCase("ZFunction/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
