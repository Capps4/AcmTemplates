#include "../../../../../src/Math/MathPackage/ModuloInteger/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
#include <sstream>

int main() {
    runCase("ModuloInteger/01-signed-min", [] {
        CHECK(Z(std::numeric_limits<long long>::min()).val() ==
              ((__int128(std::numeric_limits<long long>::min()) % P + P) % P));
    });
    runCase("ModuloInteger/02-unsigned-max", [] {
        CHECK(Z(std::numeric_limits<unsigned long long>::max()).val() ==
              int(static_cast<unsigned __int128>(std::numeric_limits<unsigned long long>::max()) %
                  P));
    });
    runCase("ModuloInteger/03-zero-power", [] {
        CHECK(Z(0).power(0) == Z(1));
        CHECK(Z(0).power(9) == Z(0));
    });
    runCase("ModuloInteger/04-negative-power", [] {
        CHECK(Z(3).power(-7) * Z(3).power(7) == Z(1));
    });
    runCase("ModuloInteger/05-wrap-add", [] {
        CHECK(Z(P - 1) + Z(P - 1) == Z(P - 2));
    });
    runCase("ModuloInteger/06-wrap-subtract", [] {
        CHECK(Z(0) - Z(P - 1) == Z(1));
    });
    runCase("ModuloInteger/07-division", [] {
        CHECK(Z(987654321) / Z(1234567) * Z(1234567) == Z(987654321));
    });
    runCase("ModuloInteger/08-wide-product", [] {
        using W = ModuloInteger<long long, 9223372036854775783LL>;
        long long p = W::getMod();
        CHECK((W(p - 1) * W(p - 2)).val() == 2);
    });
    runCase("ModuloInteger/09-quotient-correction", [] {
        using W = ModuloInteger<long long, 0>;
        W::setMod(9223372036854775807LL);
        for (long long a : {9223372036854775806LL, 4611686018427387903LL, 9007199254740993LL})
            for (long long b : {9223372036854775805LL, 4179340454199820288LL})
                CHECK((W(a) * W(b)).val() == static_cast<long long>(__int128(a) * b % W::getMod()));
    });
    runCase("ModuloInteger/10-failed-stream", [] {
        Z z = 55;
        std::istringstream in("bad");
        in >> z;
        CHECK(in.fail() and z == Z(55));
    });
    return finishCases(10);
}
