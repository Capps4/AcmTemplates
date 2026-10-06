#include "../../../../../src/String/AdvancedStrings/AcAutomaton/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>

int main() {
    AcAutomaton<128, char(128)> a(0);
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
    a.build();
    int p = 0;
    for (char c : s)
        p = a.step(p, c);
    CHECK(p == seen[3].first and p == seen[7].first);
    return 0;
}
