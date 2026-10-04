#include "Final.hpp"
#include "../TestSupport.hpp"
#include <cstring>
const char* optionsFromOtherTranslationUnit();
int enabledDebugFromOtherTranslationUnit();
int main() {
    CHECK(std::strcmp($, "color: false, space: false, precision: 6") == 0);
    CHECK($ == optionsFromOtherTranslationUnit());
    int sideEffect = 0;
    debug(++sideEffect);
    CHECK(sideEffect == 0);
    debugerOptions[0] = 'C';
    CHECK(optionsFromOtherTranslationUnit()[0] == 'C');
    debugerOptions[0] = 'c';
    CHECK(enabledDebugFromOtherTranslationUnit() == 1);
    std::cout << "Debuger disabled side effects, writable options, enabled macro, shared two-TU state PASS\n";
}
