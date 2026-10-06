#include "../../../../src/Clarketech/DebugerHeadCopyToBitsStdC++/code.hpp"
#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/CaseSupport.hpp"
int main() {
    runCase("DebugerHeadCopyToBitsStdC++/startup-integration", [] {
        CHECK($ and $[0] == '\0');
        CHECK(debuger::cfg().on and debuger::cfg().prec == 6);
        std::ostringstream out;
        auto *old = std::cerr.rdbuf(out.rdbuf());
        int x = -7;
        bool y = true;
        double z = 1.5;
        const int line = __LINE__ + 1;
        debug(7);
        const int triple = __LINE__ + 1;
        debug(x, y, z);
        const int repeated = __LINE__ + 1;
        debug(x, y, z);
        std::cerr.rdbuf(old);
        std::ostringstream expected;
        expected << std::setw(4) << line << ": 7 = 7\n";
        expected << std::setw(4) << triple << ": x, y, z = -7, true, 1.500000\n";
        expected << std::setw(4) << repeated << ": x, y, z = -7, true, 1.500000\n";
        CHECK_EQ(out.str(), expected.str());
    });
    std::free($);
    $ = nullptr;
    return 0;
}
