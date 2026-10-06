// Suppressing the injected head keeps the original startup's debug macro disabled.
#define CAPPS_DEBUGER_HEAD
#include "../../../../src/Clarketech/DebugerHeadCopyToBitsStdC++/code.hpp"
#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/CaseSupport.hpp"
int main() {
    runCase("DebugerHeadCopyToBitsStdC++/disabled-macro", [] {
        int count = 0;
        debug(++count);
        int x = 0, y = 0, z = 0;
        debug(++x, ++y, ++z);
        CHECK(x == 0 and y == 0 and z == 0);
        debug();
        CHECK(count == 0);
        CHECK($ and std::strcmp($, "color: false, space: false, precision: 6") == 0);
    });
    std::free($);
    $ = nullptr;
    return 0;
}
