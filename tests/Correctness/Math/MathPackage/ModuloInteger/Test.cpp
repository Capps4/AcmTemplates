#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/MathPackage/ModuloInteger/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <climits>
#include <sstream>


constexpr Z compileTime = (Z(2) + Z(3)) * Z(7) - Z(4);
static_assert(compileTime.val() == 31);
static_assert(Z(2).power(10).val() == 1024);
static_assert((Z(6) / Z(3)).val() == 2);
static_assert(Z(0).power(0).val() == 1);
static_assert(Z(2).power(LLONG_MIN).val() == Z(2).inv().power(LLONG_MAX).val() * 1LL * Z(2).inv().val() % P);

template<class M>
void verify(long long modulus) {
    auto norm = [&](auto value) {
        auto remainder = __int128(value) % modulus;
        return static_cast<long long>(remainder < 0 ? remainder + modulus : remainder);
    };
    CHECK(M(LLONG_MIN).val() == norm(LLONG_MIN));
    CHECK(M(ULLONG_MAX).val() == static_cast<long long>(static_cast<unsigned __int128>(ULLONG_MAX) % modulus));
    CHECK(M(__int128(LLONG_MAX) * LLONG_MAX).val() == norm(__int128(LLONG_MAX) * LLONG_MAX));
    auto wideUnsigned = (static_cast<unsigned __int128>(1) << 127) + 11;
    CHECK(M(wideUnsigned).val() == static_cast<long long>(wideUnsigned % modulus));
    for (long long a : {0LL, 1LL % modulus, modulus / 2, modulus - 1})
        for (long long b : {0LL, 1LL % modulus, modulus / 2, modulus - 1})
            CHECK((M(a) * M(b)).val() == norm(__int128(a) * b));
    for (int trial = 0; trial < 16; ++trial) {
        test_context::step = trial;
        long long a = static_cast<long long>(testRng() >> 1), b = static_cast<long long>(testRng() >> 1);
        if (trial & 1) a = -a;
        if (trial & 2) b = -b;
        M x(a), y(b);
        CHECK(x.val() == norm(a) && y.val() == norm(b));
        CHECK((x + y).val() == norm(__int128(norm(a)) + norm(b)));
        CHECK((x - y).val() == norm(__int128(norm(a)) - norm(b)));
        CHECK((x * y).val() == norm(__int128(norm(a)) * norm(b)));
        CHECK((-x).val() == norm(-__int128(norm(a))));
    }
}
int coreCases() {
    verify<Z>(P);
    using Dynamic = ModuloInteger<int, 0>;
    for (int p : {1, 2, 97, INT_MAX}) { Dynamic::setMod(p); verify<Dynamic>(p); }
    using Wide = ModuloInteger<long long, 0>;
    for (long long p : {1LL, 1000000007LL, 4179340454199820289LL, LLONG_MAX}) {
        Wide::setMod(p); verify<Wide>(p);
    }
    using FixedWide = ModuloInteger<long long, 4179340454199820289LL>;
    verify<FixedWide>(FixedWide::getMod());
    Dynamic::setMod(97);
    for (int a = 1; a < 97; ++a) {
        CHECK((Dynamic(a) * Dynamic(a).inv()).val() == 1);
        CHECK(Dynamic(a).power(-3) == Dynamic(a).inv().power(3));
    }
    Z value = 7;
    std::istringstream invalid("x"); invalid >> value;
    CHECK(value.val() == 7 && invalid.fail());
    std::istringstream valid("-5"); valid >> value;
    CHECK(value == Z(-5));
    std::ostringstream out; out << value; CHECK(out.str() == std::to_string(P - 5));
    std::cout << "constexpr arithmetic, 200K wide-integer oracles, extreme moduli, inverse and I/O failures passed\n";
    return 0;
}

#include "../../../../../src/Math/MathPackage/ModuloInteger/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
#include <sstream>

namespace boundary_cases {

int run() {
    runCase("ModuloInteger/signed-min", [] {
        CHECK(Z(std::numeric_limits<long long>::min()).val() ==
              ((__int128(std::numeric_limits<long long>::min()) % P + P) % P));
    });
    runCase("ModuloInteger/unsigned-max", [] {
        CHECK(Z(std::numeric_limits<unsigned long long>::max()).val() ==
              int(static_cast<unsigned __int128>(std::numeric_limits<unsigned long long>::max()) %
                  P));
    });
    runCase("ModuloInteger/zero-power", [] {
        CHECK(Z(0).power(0) == Z(1));
        CHECK(Z(0).power(9) == Z(0));
    });
    runCase("ModuloInteger/negative-power", [] {
        CHECK(Z(3).power(-7) * Z(3).power(7) == Z(1));
    });
    runCase("ModuloInteger/wrap-add", [] {
        CHECK(Z(P - 1) + Z(P - 1) == Z(P - 2));
    });
    runCase("ModuloInteger/wrap-subtract", [] {
        CHECK(Z(0) - Z(P - 1) == Z(1));
    });
    runCase("ModuloInteger/division", [] {
        CHECK(Z(987654321) / Z(1234567) * Z(1234567) == Z(987654321));
    });
    runCase("ModuloInteger/wide-product", [] {
        using W = ModuloInteger<long long, 9223372036854775783LL>;
        long long p = W::getMod();
        CHECK((W(p - 1) * W(p - 2)).val() == 2);
    });
    runCase("ModuloInteger/quotient-correction", [] {
        using W = ModuloInteger<long long, 0>;
        W::setMod(9223372036854775807LL);
        for (long long a : {9223372036854775806LL, 4611686018427387903LL, 9007199254740993LL})
            for (long long b : {9223372036854775805LL, 4179340454199820288LL})
                CHECK((W(a) * W(b)).val() == static_cast<long long>(__int128(a) * b % W::getMod()));
    });
    runCase("ModuloInteger/failed-stream", [] {
        Z z = 55;
        std::istringstream in("bad");
        in >> z;
        CHECK(in.fail() and z == Z(55));
    });
    return 0;
}
}

int main() {
    runCase("ModuloInteger/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
