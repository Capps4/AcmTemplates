#include "../../../../../src/String/StringF4/Kmp/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <string>

int main() {
    auto verify = [](std::string_view s) {
        int n = int(s.size());
        std::vector<int> expected(n);
        for (int i = 1; i < n; ++i) {
            for (int k = 1; k <= i; ++k)
                if (s.substr(0, k) == s.substr(i - k + 1, k)) expected[i] = k;
        }
        CHECK(kmp(s) == expected);
    };
    verify({});
    verify(std::string_view("\0ab\0ab", 6));
    for (int n = 0; n <= 11; ++n) {
        for (int mask = 0; mask < (1 << n); ++mask) {
            std::string s(n, 'a');
            for (int i = 0; i < n; ++i) s[i] += (mask >> i) & 1;
            verify(s);
        }
    }
    for (int trial = 0; trial < 1500; ++trial) {
        std::string text(randomInt(0, 100), 'a');
        for (auto& c : text) c = char(randomInt(0, 5));
        int l = randomInt(0, int(text.size())), r = randomInt(l, int(text.size()));
        verify(std::string_view(text).substr(l, r - l));
    }
    std::string equal(1000000, 'a');
    auto result = kmp(equal);
    for (int i = 1; i < int(equal.size()); ++i)
        CHECK(result[i] == i);
    std::cout << "Exhaustive binary strings, random slices, embedded NUL and 1M repeated characters passed\n";
}
