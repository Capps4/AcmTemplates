#if __has_include(<bits/stdc++.h>)
#include <bits/stdc++.h>
#else
#include <cstring>
inline bool mockConfigured = false;
const char* mockConfigure(const char* value) { mockConfigured = true; return value; }
#define COMPETITION_DEBUGER
#define strdup mockConfigure
#define debug(value) ((void)(value))
#endif
#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/TestSupport.hpp"
int main() {
#if __has_include(<bits/stdc++.h>)
    CHECK(_$competition_debug::debug_enabled_flag);
    CHECK(!_$competition_debug::output_color_enabled && !_$competition_debug::output_indent_enabled);
    CHECK(_$competition_debug::debug_precision == 6);
    CHECK(std::strcmp($, "") == 0);
#else
    CHECK(mockConfigured);
    CHECK(std::strcmp($, "color: false, space: false, precision: 6") == 0);
#endif
    int value = 7;
    debug(value);
    std::cout << "Debuger actual local GNU plugin / Clang registration fixture PASS\n";
}
