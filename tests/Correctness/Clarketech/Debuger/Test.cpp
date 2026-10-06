#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/TestSupport.hpp"
int main() {
    CHECK(std::strcmp($, "color: false, space: false, precision: 6") == 0);
    int cnt = 0;
    debug(++cnt);
    CHECK(cnt == 0);
    std::free($);
    std::cout << "Debuger exact options and disabled side effects PASS\n";
}
