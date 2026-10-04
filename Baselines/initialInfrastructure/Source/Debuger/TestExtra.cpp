#define COMPETITION_DEBUGER
#define debug(value) ++(value)
#include "Final.hpp"
const char* optionsFromOtherTranslationUnit() { return $; }
int enabledDebugFromOtherTranslationUnit() {
    int count = 0;
    debug(count);
    return count;
}
