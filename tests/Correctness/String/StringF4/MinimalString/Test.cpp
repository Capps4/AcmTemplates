#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/String/StringF4/MinimalString/code.hpp"
#include "../../../../Support/TestSupport.hpp"

template<class Sequence, class Less>
auto naive(const Sequence& input, Less less) {
    auto expected = input;
    for (std::size_t i = 1; i < input.size(); ++i) {
        auto candidate = input;
        std::rotate(candidate.begin(), candidate.begin() + i, candidate.end());
        if (std::lexicographical_compare(candidate.begin(), candidate.end(), expected.begin(), expected.end(), less))
            expected = std::move(candidate);
    }
    return expected;
}
void verify(const std::string& input) {
    auto expected = naive(input, [](char a, char b) { return static_cast<unsigned char>(a) < static_cast<unsigned char>(b); });
    CHECK((input | minimalString()) == expected);
    CHECK((std::string_view(input) | minimalString()) == expected);
    CHECK((std::string(input) | minimalString()) == expected);
}
int coreCases() {
    for (int n = 0; n <= 5; ++n)
        for (int mask = 0; mask < (1 << n); ++mask) {
            std::string s(n, 'a');
            for (int i = 0; i < n; ++i) s[i] += bool(mask & (1 << i));
            verify(s);
        }
    for (int test = 0; test < 16; ++test) {
        test_context::step = test;
        std::string s(randomInt(0, 100), '\0');
        for (auto& c : s) c = char(randomInt(0, 255));
        verify(s);
    }
    std::vector<int> values{5, -1, 0, -1};
    CHECK((values | minimalString()) == naive(values, std::less<>{}));
    std::vector<bool> bits{true, false, true, false};
    CHECK((bits | minimalString()) == std::vector<bool>({false, true, false, true}));
    std::string repeated(128, 'x');
    CHECK((repeated | minimalString()) == repeated);
    std::cout << "MinimalString: exhaustive rotations, arbitrary bytes, empty/views, integer/bool and million repetition PASS\n";
    return 0;
}

#include "../../../../../src/String/StringF4/MinimalString/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
template <class Sequence, class Less>
auto naive(const Sequence &input, Less less) {
    auto expected = input;
    for (std::size_t i = 1; i < input.size(); ++i) {
        auto candidate = input;
        std::rotate(candidate.begin(), candidate.begin() + i, candidate.end());
        if (std::lexicographical_compare(candidate.begin(), candidate.end(), expected.begin(),
                                         expected.end(), less))
            expected = std::move(candidate);
    }
    return expected;
}
void verify(const std::string &input) {
    auto expected = naive(input, [](char a, char b) {
        return static_cast<unsigned char>(a) < static_cast<unsigned char>(b);
    });
    CHECK((input | minimalString()) == expected);
    CHECK((std::string_view(input) | minimalString()) == expected);
    CHECK((std::string(input) | minimalString()) == expected);
}

int run() {
    runCase("MinimalString/overlap", [] {
        verify("aaabaaa");
    });
    runCase("MinimalString/periodic-tail", [] {
        verify("abcabcab");
    });
    runCase("MinimalString/alphabet", [] {
        verify("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("MinimalString/even-palindromes", [] {
        verify("abbaabba");
    });
    runCase("MinimalString/byte-domain", [] {
        verify(std::string("\0\xff\x80\0", 4));
    });
    return 0;
}
}

int main() {
    runCase("MinimalString/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
