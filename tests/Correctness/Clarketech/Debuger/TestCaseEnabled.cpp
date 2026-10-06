#define COMPETITION_DEBUGER
#define debug(value) ++(value)
#include "../../../../src/Clarketech/Debuger/code.hpp"
const char* optionsFromOtherTranslationUnit() { return $; }
int enabledDebugFromOtherTranslationUnit() {
    int count = 0;
    debug(count);
    return count;
}

int main() { int cnt = enabledDebugFromOtherTranslationUnit(); std::free($); return cnt != 1; }
