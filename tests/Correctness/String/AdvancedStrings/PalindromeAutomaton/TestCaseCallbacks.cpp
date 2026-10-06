#include "../../../../../src/String/AdvancedStrings/PalindromeAutomaton/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>

int main() {
    Pam<128, char(128)> a(0);
    int calls = 0;
    std::vector<std::pair<int, int>> seen;
    auto fn = [&, own = std::make_unique<int>(7)](int p, int i) {
        CHECK(*own == 7);
        seen.emplace_back(p, i);
        ++calls;
    };
    static_assert(not std::is_copy_constructible_v<decltype(fn)>);
    std::string s("\x80\xff\xff\x80", 4);
    a.add("", fn);
    CHECK(calls == 0);
    a.add(s, fn);
    CHECK(calls == 4);
    for (int i = 0; i < 4; ++i) {
        CHECK(seen[i].second == i);
        CHECK(seen[i].first > 0);
    }
    a.add(s, fn); // Reuse the same noncopyable lvalue callback; i resets to zero.
    CHECK(calls == 8);
    for (int i = 0; i < 4; ++i)
        CHECK(seen[i + 4].second == i);
    CHECK(a.s == s + s);
    CHECK(a.len[0] == 0 and a.len[1] == -1);
    CHECK(a.link[0] == 1 and a.link[1] == 1);
    return 0;
}
