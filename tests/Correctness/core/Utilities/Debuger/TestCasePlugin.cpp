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
#include "../../../../../src/core/Utilities/Debuger/code.hpp"
#include "../../../../Support/TestSupport.hpp"
int isolatedCase() {
#if __has_include(<bits/stdc++.h>)
    CHECK(debuger::cfg().on);
    CHECK(!debuger::cfg().col and !debuger::cfg().gap);
    CHECK(debuger::cfg().prec == 6);
    CHECK(std::strcmp($, "") == 0);
#else
    CHECK(mockConfigured);
    CHECK(std::strcmp($, "color: false, space: false, precision: 6") == 0);
#endif
    int value = 7;
    debug(value);
    std::cout << "Debuger actual local GNU plugin / Clang registration fixture PASS\n";
    return 0;
}

#include "../../../../Support/CaseSupport.hpp"
int main() {
    runCase("Debuger/plugin", [] { CHECK(isolatedCase() == 0); });
    return 0;
}
