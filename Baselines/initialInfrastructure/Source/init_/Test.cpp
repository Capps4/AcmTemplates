#include <iostream>
#define main snippetMain
#include "Final.hpp"
#undef main
#include "../TestSupport.hpp"
int main() {
    CHECK(snippetMain() == 0);
    CHECK(std::cin.tie() == nullptr);
    CHECK(std::ios::sync_with_stdio(false) == false);
    static_assert(std::is_same_v<i64, long long>);
    std::cout << "init_ final entry settings/return/type aliases PASS; actual exported cursor expansion verified by Integration.py\n";
}
