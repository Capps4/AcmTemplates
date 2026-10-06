#define COMPETITION_DEBUGER
#define debug(value) ++(value)
#include "../../../../src/Clarketech/Debuger/code.hpp"
const char* optionsFromOtherTranslationUnit() { return $; }
int enabledDebugFromOtherTranslationUnit() {
    int count = 0;
    debug(count);
    return count;
}

int isolatedCase() {
    int cnt = enabledDebugFromOtherTranslationUnit();
    std::free($);
    return cnt != 1;
}

#include "../../../Support/CaseSupport.hpp"
int main() {
    runCase("Debuger/enabled", [] { CHECK(isolatedCase() == 0); });
    return 0;
}
