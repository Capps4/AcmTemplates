#include <iostream>
#include <sstream>
#define main snippetMain
#include "Final.hpp"
#undef main
#include "../TestSupport.hpp"
int main() {
    std::ios::sync_with_stdio(false);
    for (const char* text : {"", "bad", "-1", "-2147483648", "0", "3"}) {
        std::istringstream input(text);
        auto old = std::cin.rdbuf(input.rdbuf());
        std::cin.clear();
        CHECK(snippetMain() == 0);
        CHECK(std::cin.tie() == nullptr);
        std::cin.rdbuf(old); std::cin.clear();
    }
    std::cout << "init_case empty/malformed/negative/zero/positive count PASS; solve count and exported cursor expansion verified by Integration.py\n";
}
