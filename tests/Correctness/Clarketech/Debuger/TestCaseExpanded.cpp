#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/CaseSupport.hpp"

int main() {
    runCase("Debuger/01-exact-options", [] {
        CHECK(std::string($) == "color: false, space: false, precision: 6");
    });
    runCase("Debuger/02-option-length", [] {
        CHECK(std::strlen($) == 40);
    });
    runCase("Debuger/03-owned-mutable-copy", [] {
        char c = $[0];
        $[0] = 'C';
        CHECK($[0] == 'C');
        $[0] = c;
    });
    runCase("Debuger/04-one-side-effect", [] {
        int n = 0;
        debug(++n);
        CHECK(n == 0);
    });
    runCase("Debuger/05-variadic-side-effects", [] {
        int a = 0, b = 0;
        debug(++a, ++b);
        CHECK(a == 0 and b == 0);
    });
    runCase("Debuger/06-throw-expression", [] {
        debug(throw std::runtime_error("disabled"));
        CHECK(true);
    });
    runCase("Debuger/07-unknown-expression", [] {
        debug(this_name_is_intentionally_not_defined);
        CHECK(true);
    });
    runCase("Debuger/08-empty-call", [] {
        debug();
        CHECK($[0] == 'c');
    });
    runCase("Debuger/09-loop-disabled", [] {
        int n = 0;
        for (int i = 0; i < 1000; ++i) {
            debug(n += i);
        }
        CHECK(n == 0);
    });
    runCase("Debuger/10-release-config", [] {
        CHECK($[std::strlen($)] == 0);
        std::free($);
        $ = nullptr;
        CHECK($ == nullptr);
    });
    return finishCases(10);
}
