// An existing COMPETITION_DEBUGER provider must retain its macros and symbols.
#define COMPETITION_DEBUGER
#define debug(...) (++(__VA_ARGS__))
#include "../../../../src/Clarketech/DebugerHeadCopyToBitsStdC++/code.hpp"
#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/CaseSupport.hpp"
int main() {
    runCase("DebugerHeadCopyToBitsStdC++/existing-provider", [] {
        int count = 0;
        debug(count);
        CHECK(count == 1);
        CHECK($ and std::strcmp($, "color: false, space: false, precision: 6") == 0);
    });
    std::free($);
    $ = nullptr;
    return 0;
}
