#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/TestSupport.hpp"

#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/CaseSupport.hpp"

namespace boundary_cases {

int run() {
    runCase("Debuger/exact-options", [] {
        CHECK(std::string($) == "color: false, space: false, precision: 6");
    });
    runCase("Debuger/option-length", [] {
        CHECK(std::strlen($) == 40);
    });
    runCase("Debuger/owned-mutable-copy", [] {
        char c = $[0];
        $[0] = 'C';
        CHECK($[0] == 'C');
        $[0] = c;
    });
    runCase("Debuger/one-side-effect", [] {
        int n = 0;
        debug(++n);
        CHECK(n == 0);
    });
    runCase("Debuger/variadic-side-effects", [] {
        int a = 0, b = 0;
        debug(++a, ++b);
        CHECK(a == 0 and b == 0);
    });
    runCase("Debuger/throw-expression", [] {
        debug(throw std::runtime_error("disabled"));
        CHECK(true);
    });
    runCase("Debuger/unknown-expression", [] {
        debug(this_name_is_intentionally_not_defined);
        CHECK(true);
    });
    runCase("Debuger/empty-call", [] {
        debug();
        CHECK($[0] == 'c');
    });
    runCase("Debuger/loop-disabled", [] {
        int n = 0;
        for (int i = 0; i < 1000; ++i) {
            debug(n += i);
        }
        CHECK(n == 0);
    });
    runCase("Debuger/release-config", [] {
        CHECK($[std::strlen($)] == 0);
        std::free($);
        $ = nullptr;
        CHECK($ == nullptr);
    });
    return 0;
}
}

int main() {
    CHECK(boundary_cases::run() == 0);
    return 0;
}
